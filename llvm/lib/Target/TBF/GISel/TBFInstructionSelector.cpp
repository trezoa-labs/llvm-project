//===- BPFInstructionSelector.cpp --------------------------------*- C++ -*-==//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the InstructionSelector class for BPF.
//===----------------------------------------------------------------------===//

#include "TBFInstrInfo.h"
#include "TBFSubtarget.h"
#include "TBFTargetMachine.h"
#include "TBFRegisterBankInfo.h"
#include "llvm/CodeGen/GlobalISel/GIMatchTableExecutorImpl.h"
#include "llvm/CodeGen/GlobalISel/InstructionSelector.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/IR/IntrinsicsBPF.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "TBF-gisel"

using namespace llvm;

namespace {

#define GET_GLOBALISEL_PREDICATE_BITSET
#include "TBFGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATE_BITSET

class TBFInstructionSelector : public InstructionSelector {
public:
  TBFInstructionSelector(const TBFTargetMachine &TM, const TBFSubtarget &STI,
                         const TBFRegisterBankInfo &RBI);

  bool select(MachineInstr &I) override;
  static const char *getName() { return DEBUG_TYPE; }

private:
  /// tblgen generated 'select' implementation that is used as the initial
  /// selector for the patterns that do not require complex C++.
  bool selectImpl(MachineInstr &I, CodeGenCoverage &CoverageInfo) const;

  const TBFInstrInfo &TII;
  const TBFRegisterInfo &TRI;
  const TBFRegisterBankInfo &RBI;

#define GET_GLOBALISEL_PREDICATES_DECL
#include "TBFGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_DECL

#define GET_GLOBALISEL_TEMPORARIES_DECL
#include "TBFGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_DECL
};

} // namespace

#define GET_GLOBALISEL_IMPL
#include "TBFGenGlobalISel.inc"
#undef GET_GLOBALISEL_IMPL

TBFInstructionSelector::TBFInstructionSelector(const TBFTargetMachine &TM,
                                               const TBFSubtarget &STI,
                                               const TBFRegisterBankInfo &RBI)
    : TII(*STI.getInstrInfo()), TRI(*STI.getRegisterInfo()), RBI(RBI),
#define GET_GLOBALISEL_PREDICATES_INIT
#include "TBFGenGlobalISel.inc"
#undef GET_GLOBALISEL_PREDICATES_INIT
#define GET_GLOBALISEL_TEMPORARIES_INIT
#include "TBFGenGlobalISel.inc"
#undef GET_GLOBALISEL_TEMPORARIES_INIT
{
}

bool TBFInstructionSelector::select(MachineInstr &I) {
  if (!isPreISelGenericOpcode(I.getOpcode()))
    return true;
  if (selectImpl(I, *CoverageInfo))
    return true;
  return false;
}

namespace llvm {
InstructionSelector *
createTBFInstructionSelector(const TBFTargetMachine &TM,
                             const TBFSubtarget &Subtarget,
                             const TBFRegisterBankInfo &RBI) {
  return new TBFInstructionSelector(TM, Subtarget, RBI);
}
} // namespace llvm
