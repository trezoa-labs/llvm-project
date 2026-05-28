//===-- TBF.h - Top-level interface for TBF representation ------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_SBF_H
#define LLVM_LIB_TARGET_TBF_SBF_H

#include "MCTargetDesc/TBFMCTargetDesc.h"
#include "llvm/IR/PassManager.h"
#include "llvm/Pass.h"
#include "llvm/Target/TargetMachine.h"

namespace llvm {
class TBFRegisterBankInfo;
class TBFSubtarget;
class TBFTargetMachine;
class InstructionSelector;
class TBFTargetMachine;
class PassRegistry;

ModulePass *createTBFCheckAndAdjustIR();

FunctionPass *createTBFISelDag(TBFTargetMachine &TM);
FunctionPass *createTBFMISimplifyPatchablePass();
FunctionPass *createTBFMIPeepholePass();
FunctionPass *createTBFMIPeepholeTruncElimPass();
FunctionPass *createTBFMIPreEmitPeepholePass(CodeGenOptLevel OptLevel,
                                             bool DisablePeephole);
FunctionPass *createTBFMIPreEmitCheckingPass();

InstructionSelector *createTBFInstructionSelector(const TBFTargetMachine &,
                                                  const TBFSubtarget &,
                                                  const TBFRegisterBankInfo &);

void initializeTBFCheckAndAdjustIRPass(PassRegistry&);
void initializeTBFDAGToDAGISelLegacyPass(PassRegistry &);
void initializeTBFMIPeepholePass(PassRegistry&);
void initializeTBFMIPeepholeTruncElimPass(PassRegistry &);
void initializeTBFMIPreEmitCheckingPass(PassRegistry&);
void initializeTBFMIPreEmitPeepholePass(PassRegistry &);
void initializeTBFMISimplifyPatchablePass(PassRegistry &);

class TBFAbstractMemberAccessPass
    : public PassInfoMixin<TBFAbstractMemberAccessPass> {
  TBFTargetMachine *TM;

public:
  TBFAbstractMemberAccessPass(TBFTargetMachine *TM) : TM(TM) {}
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

  static bool isRequired() { return true; }
};

class TBFPreserveDITypePass : public PassInfoMixin<TBFPreserveDITypePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

  static bool isRequired() { return true; }
};

class TBFIRPeepholePass : public PassInfoMixin<TBFIRPeepholePass> {
public:
  PreservedAnalyses run(Function &F, FunctionAnalysisManager &AM);

  static bool isRequired() { return true; }
};
} // namespace llvm

#endif
