//===-- TBFFrameLowering.h - Define frame lowering for TBF -----*- C++ -*--===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class implements TBF-specific bits of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_TBFFRAMELOWERING_H
#define LLVM_LIB_TARGET_TBF_TBFFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class TBFSubtarget;

class TBFFrameLowering : public TargetFrameLowering {
public:
  explicit TBFFrameLowering(const TBFSubtarget &sti)
      : TargetFrameLowering(
            TargetFrameLowering::StackGrowsDown,
            Align(64),
            0,
            Align(64)) {}

  void emitPrologue(MachineFunction &MF, MachineBasicBlock &MBB) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  bool hasFPImpl(const MachineFunction &MF) const override;
  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator MI) const override {
    return MBB.erase(MI);
  }
private:
  const int FrameSize = 4096;
};
} // namespace llvm
#endif
