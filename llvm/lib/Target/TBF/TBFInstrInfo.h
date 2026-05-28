//===-- TBFInstrInfo.h - TBF Instruction Information ------------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TBF implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_TBFINSTRINFO_H
#define LLVM_LIB_TARGET_TBF_TBFINSTRINFO_H

#include "TBFRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"

#define GET_INSTRINFO_HEADER
#include "TBFGenInstrInfo.inc"

namespace llvm {

class TBFInstrInfo : public TBFGenInstrInfo {
  const TBFRegisterInfo RI;

public:
  TBFInstrInfo();

  const TBFRegisterInfo &getRegisterInfo() const { return RI; }

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator I,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  void storeRegToStackSlot(
      MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, Register SrcReg,
      bool isKill, int FrameIndex, const TargetRegisterClass *RC,
      const TargetRegisterInfo *TRI, Register VReg,
      MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;

  void loadRegFromStackSlot(
      MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
      Register DestReg, int FrameIndex, const TargetRegisterClass *RC,
      const TargetRegisterInfo *TRI, Register VReg,
      MachineInstr::MIFlag Flags = MachineInstr::NoFlags) const override;

  bool analyzeBranch(MachineBasicBlock &MBB, MachineBasicBlock *&TBB,
                     MachineBasicBlock *&FBB,
                     SmallVectorImpl<MachineOperand> &Cond,
                     bool AllowModify) const override;

  unsigned removeBranch(MachineBasicBlock &MBB,
                        int *BytesRemoved = nullptr) const override;
  unsigned insertBranch(MachineBasicBlock &MBB, MachineBasicBlock *TBB,
                        MachineBasicBlock *FBB, ArrayRef<MachineOperand> Cond,
                        const DebugLoc &DL,
                        int *BytesAdded = nullptr) const override;
  void initializeTargetFeatures(bool HasExplicitSext, bool NewMemEncoding);

  bool
  reverseBranchCondition(SmallVectorImpl<MachineOperand> &Cond) const override;

  MachineBasicBlock *getBranchDestBlock(const MachineInstr &MI) const override;

  unsigned getInstSizeInBytes(const MachineInstr &MI) const override;

  std::optional<RegImmPair> isAddImmediate(const MachineInstr &MI,
                                           Register Reg) const override;

  Register isStoreToStackSlot(const MachineInstr &MI,
                              int &FrameIndex) const override;

  Register isStoreToStackSlot(const MachineInstr &MI, int &FrameIndex,
                              unsigned &MemBytes) const override;

  Register isLoadFromStackSlot(const MachineInstr &MI,
                               int &FrameIndex) const override;

  Register isLoadFromStackSlot(const MachineInstr &MI, int &FrameIndex,
                               unsigned &MemBytes) const override;

private:
  bool HasExplicitSignExt;
  bool NewMemEncoding;
};
}

#endif
