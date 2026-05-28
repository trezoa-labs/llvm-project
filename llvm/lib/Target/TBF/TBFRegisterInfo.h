//===-- TBFRegisterInfo.h - TBF Register Information Impl -------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TBF implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_TBFREGISTERINFO_H
#define LLVM_LIB_TARGET_TBF_TBFREGISTERINFO_H

#include "llvm/CodeGen/TargetRegisterInfo.h"
#include <optional>

#define GET_REGINFO_HEADER
#include "TBFGenRegisterInfo.inc"

namespace llvm {

struct TBFRegisterInfo : public TBFGenRegisterInfo {
  static unsigned FrameLength;

  TBFRegisterInfo();

  const MCPhysReg *getCalleeSavedRegs(const MachineFunction *MF) const override;

  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;

  int resolveInternalFrameIndex(const MachineFunction &MF, int FI,
                                std::optional<int64_t> Imm) const;
};
}

#endif
