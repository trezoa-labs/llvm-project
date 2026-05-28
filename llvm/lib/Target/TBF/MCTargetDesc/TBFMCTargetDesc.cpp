//===-- TBFMCTargetDesc.cpp - TBF Target Descriptions ---------------------===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides TBF specific target descriptions.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TBFMCTargetDesc.h"
#include "MCTargetDesc/TBFInstPrinter.h"
#include "MCTargetDesc/TBFMCAsmInfo.h"
#include "TargetInfo/TBFTargetInfo.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCCodeEmitter.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCELFStreamer.h"
#include "llvm/MC/MCInstrAnalysis.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/TargetParser/Host.h"

#define GET_INSTRINFO_MC_DESC
#define ENABLE_INSTR_PREDICATE_VERIFIER
#include "TBFGenInstrInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "TBFGenSubtargetInfo.inc"

#define GET_REGINFO_MC_DESC
#include "TBFGenRegisterInfo.inc"

using namespace llvm;

static MCInstrInfo *createTBFMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitTBFMCInstrInfo(X);
  return X;
}

static MCRegisterInfo *createTBFMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitTBFMCRegisterInfo(X, TBF::R10 /* RAReg doesn't exist */);
  return X;
}

static MCSubtargetInfo *createTBFMCSubtargetInfo(const Triple &TT,
                                                 StringRef CPU, StringRef FS) {
  std::string CpuStr = cpuFromSubArch(TT, CPU.str());
  StringRef CpuRef = CpuStr;
  return createTBFMCSubtargetInfoImpl(TT, CpuRef, /*TuneCPU*/ CpuRef, FS);
}

static MCStreamer *createTBFMCStreamer(const Triple &T, MCContext &Ctx,
                                       std::unique_ptr<MCAsmBackend> &&MAB,
                                       std::unique_ptr<MCObjectWriter> &&OW,
                                       std::unique_ptr<MCCodeEmitter> &&Emitter) {
  MCELFStreamer *S =
      new MCELFStreamer(Ctx, std::move(MAB), std::move(OW), std::move(Emitter));

  const MCSubtargetInfo *STI = Ctx.getSubtargetInfo();

  StringRef CPU = STI->getCPU();
  unsigned EFlag = llvm::ELF::EF_TBF_V0;
  if (CPU == "v1") {
    EFlag = llvm::ELF::EF_TBF_V1;
  } else if (CPU == "v2") {
    EFlag = llvm::ELF::EF_TBF_V2;
  } else if (CPU == "v3") {
    EFlag = llvm::ELF::EF_TBF_V3;
  } else if (CPU == "v4") {
    EFlag = llvm::ELF::EF_TBF_V4;
  }
  S->getWriter().setELFHeaderEFlags(EFlag);

  return S;
}

static MCInstPrinter *createTBFMCInstPrinter(const Triple &T,
                                             unsigned SyntaxVariant,
                                             const MCAsmInfo &MAI,
                                             const MCInstrInfo &MII,
                                             const MCRegisterInfo &MRI) {
  if (SyntaxVariant == 0)
    return new TBFInstPrinter(MAI, MII, MRI);

  return nullptr;
}

namespace {

class TBFMCInstrAnalysis : public MCInstrAnalysis {
public:
  explicit TBFMCInstrAnalysis(const MCInstrInfo *Info)
      : MCInstrAnalysis(Info) {}

  bool evaluateBranch(const MCInst &Inst, uint64_t Addr, uint64_t Size,
                      uint64_t &Target) const override {
    // The target is the 3rd operand of cond inst and the 1st of uncond inst.
    int16_t Imm;
    if (isConditionalBranch(Inst)) {
      Imm = Inst.getOperand(2).getImm();
    } else if (isUnconditionalBranch(Inst))
      Imm = Inst.getOperand(0).getImm();
    else
      return false;

    Target = Addr + Size + Imm * Size;
    return true;
  }
};

} // end anonymous namespace

static MCInstrAnalysis *createTBFInstrAnalysis(const MCInstrInfo *Info) {
  return new TBFMCInstrAnalysis(Info);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTBFTargetMC() {
  for (Target *T : {&getTheTBFXTarget()}) {
    // Register the MC asm info.
    RegisterMCAsmInfo<TBFMCAsmInfo> X(*T);

    // Register the MC instruction info.
    TargetRegistry::RegisterMCInstrInfo(*T, createTBFMCInstrInfo);

    // Register the MC register info.
    TargetRegistry::RegisterMCRegInfo(*T, createTBFMCRegisterInfo);

    // Register the MC subtarget info.
    TargetRegistry::RegisterMCSubtargetInfo(*T,
                                            createTBFMCSubtargetInfo);

    // Register the object streamer
    TargetRegistry::RegisterELFStreamer(*T, createTBFMCStreamer);

    // Register the MCInstPrinter.
    TargetRegistry::RegisterMCInstPrinter(*T, createTBFMCInstPrinter);

    // Register the MC instruction analyzer.
    TargetRegistry::RegisterMCInstrAnalysis(*T, createTBFInstrAnalysis);
  }

  // Register the MC code emitter
  TargetRegistry::RegisterMCCodeEmitter(getTheTBFXTarget(),
                                        createTBFMCCodeEmitter);

  // Register the ASM Backend
  TargetRegistry::RegisterMCAsmBackend(getTheTBFXTarget(),
                                       createTBFAsmBackend);
}
