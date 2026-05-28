//===-- TBFTargetMachine.cpp - Define TargetMachine for TBF ---------------===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about TBF target spec.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TBFMCAsmInfo.h"
#include "TBF.h"
#include "TBFFunctionInfo.h"
#include "TBFTargetMachine.h"
#include "TBFTargetTransformInfo.h"
#include "TargetInfo/TBFTargetInfo.h"
#include "llvm/CodeGen/ExpandMemCmp.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/IR/PassManager.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/SimplifyCFG.h"
#include "llvm/Transforms/Utils/SimplifyCFGOptions.h"
#include <optional>
using namespace llvm;

static cl::
opt<bool> DisableMIPeephole("disable-tbf-peephole", cl::Hidden,
                            cl::desc("Disable machine peepholes for TBF"));

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTBFTarget() {
  // Register the target.
  RegisterTargetMachine<TBFTargetMachine> XX(getTheTBFXTarget());

  PassRegistry &PR = *PassRegistry::getPassRegistry();
  initializeTBFCheckAndAdjustIRPass(PR);
  initializeTBFMIPeepholePass(PR);
  initializeTBFMIPeepholeTruncElimPass(PR);
  initializeTBFDAGToDAGISelLegacyPass(PR);
}

// DataLayout: little or big endian
static std::string computeDataLayout(const Triple &TT, StringRef FS) {
  // TOOD: jle; specialize this (and elsewhere) to Trezoa-only once the new
  // back-end is integrated; e.g. we won't need IsTrezoa, etc.
  assert(TT.getArch() == Triple::tbf && "expected Triple::tbf");
  return "e-m:e-p:64:64-i64:64-n32:64-S128";
}

static Reloc::Model getEffectiveRelocModel(std::optional<Reloc::Model> RM) {
  return RM.value_or(Reloc::PIC_);
}

TBFTargetMachine::TBFTargetMachine(const Target &T, const Triple &TT,
                                   StringRef CPU, StringRef FS,
                                   const TargetOptions &Options,
                                   std::optional<Reloc::Model> RM,
                                   std::optional<CodeModel::Model> CM,
                                   CodeGenOptLevel OL, bool JIT)
    : CodeGenTargetMachineImpl(T, computeDataLayout(TT, FS), TT, CPU, FS, Options,
                        getEffectiveRelocModel(RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()),
      Subtarget(TT, std::string(CPU), std::string(FS), *this) {
  initAsmInfo();

  TBFMCAsmInfo *MAI =
      static_cast<TBFMCAsmInfo *>(const_cast<MCAsmInfo *>(AsmInfo.get()));
  MAI->setDwarfUsesRelocationsAcrossSections(!Subtarget.getUseDwarfRIS());
  MAI->setSupportsDebugInformation(true);
}

namespace {
// TBF Code Generator Pass Configuration Options.
class TBFPassConfig : public TargetPassConfig {
public:
  TBFPassConfig(TBFTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  TBFTargetMachine &getTBFTargetMachine() const {
    return getTM<TBFTargetMachine>();
  }

  void addIRPasses() override;
  bool addInstSelector() override;
  void addMachineSSAOptimization() override;
  void addPreEmitPass() override;
};
}

TargetPassConfig *TBFTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new TBFPassConfig(*this, PM);
}

void TBFTargetMachine::registerPassBuilderCallbacks(PassBuilder &PB) {
  PB.registerPipelineParsingCallback(
      [](StringRef PassName, FunctionPassManager &FPM,
         ArrayRef<PassBuilder::PipelineElement>) {
        if (PassName == "tbf-ir-peephole") {
          FPM.addPass(TBFIRPeepholePass());
          return true;
        }
        return false;
      });
  PB.registerPipelineStartEPCallback(
      [=](ModulePassManager &MPM, OptimizationLevel) {
        FunctionPassManager FPM;
        FPM.addPass(TBFAbstractMemberAccessPass(this));
        FPM.addPass(TBFPreserveDITypePass());
        FPM.addPass(TBFIRPeepholePass());
        MPM.addPass(createModuleToFunctionPassAdaptor(std::move(FPM)));
      });
  PB.registerPeepholeEPCallback([=](FunctionPassManager &FPM,
                                    OptimizationLevel Level) {
    FPM.addPass(ExpandMemCmpPass(this));
    FPM.addPass(InstCombinePass());
    FPM.addPass(SimplifyCFGPass(
        SimplifyCFGOptions().hoistCommonInsts(true).convertSwitchToLookupTable(
            true)));
  });
}

void TBFPassConfig::addIRPasses() {
  addPass(createTBFCheckAndAdjustIR());
  TargetPassConfig::addIRPasses();
}

TargetTransformInfo
TBFTargetMachine::getTargetTransformInfo(const Function &F) const {
  return TargetTransformInfo(TBFTTIImpl(this, F));
}

MachineFunctionInfo *TBFTargetMachine::createMachineFunctionInfo(
    llvm::BumpPtrAllocator &Allocator, const llvm::Function &F,
    const llvm::TargetSubtargetInfo *STI) const {
  return TBFFunctionInfo::create<TBFFunctionInfo>(
      Allocator, F, static_cast<const TBFSubtarget *>(STI));
}

// Install an instruction selector pass using
// the ISelDag to gen TBF code.
bool TBFPassConfig::addInstSelector() {
  addPass(createTBFISelDag(getTBFTargetMachine()));

  return false;
}

void TBFPassConfig::addMachineSSAOptimization() {
  addPass(createTBFMISimplifyPatchablePass());

  // The default implementation must be called first as we want eBPF
  // Peephole ran at last.
  TargetPassConfig::addMachineSSAOptimization();

  const TBFSubtarget *Subtarget = getTBFTargetMachine().getSubtargetImpl();
  if (!DisableMIPeephole) {
    if (Subtarget->getHasAlu32() && Subtarget->getHasExplicitSignExt())
      addPass(createTBFMIPeepholePass());
    addPass(createTBFMIPeepholeTruncElimPass());
  }
}

void TBFPassConfig::addPreEmitPass() {
  addPass(createTBFMIPreEmitCheckingPass());
  addPass(createTBFMIPreEmitPeepholePass(getOptLevel(), DisableMIPeephole));
}
