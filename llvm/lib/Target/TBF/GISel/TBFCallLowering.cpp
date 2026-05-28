//===-- BPFCallLowering.cpp - Call lowering for GlobalISel ------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements the lowering of LLVM calls to machine code calls for
/// GlobalISel.
///
//===----------------------------------------------------------------------===//

#include "TBFCallLowering.h"
#include "llvm/CodeGen/GlobalISel/MachineIRBuilder.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "tbf-call-lowering"

using namespace llvm;

TBFCallLowering::TBFCallLowering(const TBFTargetLowering &TLI)
    : CallLowering(&TLI) {}

bool TBFCallLowering::lowerReturn(MachineIRBuilder &MIRBuilder,
                                  const Value *Val, ArrayRef<Register> VRegs,
                                  FunctionLoweringInfo &FLI,
                                  Register SwiftErrorVReg) const {
  if (!VRegs.empty())
    return false;
  MIRBuilder.buildInstr(TBF::EXIT);
  return true;
}

bool TBFCallLowering::lowerFormalArguments(MachineIRBuilder &MIRBuilder,
                                           const Function &F,
                                           ArrayRef<ArrayRef<Register>> VRegs,
                                           FunctionLoweringInfo &FLI) const {
  return VRegs.empty();
}

bool TBFCallLowering::lowerCall(MachineIRBuilder &MIRBuilder,
                                CallLoweringInfo &Info) const {
  return false;
}
