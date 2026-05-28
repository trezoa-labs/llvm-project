//===-- TBFTargetMachine.h - Define TargetMachine for TBF --- C++ ---===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the TBF specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_TBFTARGETMACHINE_H
#define LLVM_LIB_TARGET_TBF_TBFTARGETMACHINE_H

#include "TBFSubtarget.h"
#include "llvm/CodeGen/CodeGenTargetMachineImpl.h"

namespace llvm {
class TBFTargetMachine : public CodeGenTargetMachineImpl {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
  TBFSubtarget Subtarget;

public:
  TBFTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                   StringRef FS, const TargetOptions &Options,
                   std::optional<Reloc::Model> RM,
                   std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                   bool JIT);

  const TBFSubtarget *getSubtargetImpl() const { return &Subtarget; }
  const TBFSubtarget *getSubtargetImpl(const Function &) const override {
    return &Subtarget;
  }

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetTransformInfo getTargetTransformInfo(const Function &F) const override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

  void registerPassBuilderCallbacks(PassBuilder &PB) override;

  MachineFunctionInfo *
  createMachineFunctionInfo(BumpPtrAllocator &Allocator, const Function &F,
                            const TargetSubtargetInfo *STI) const override;
};
}

#endif
