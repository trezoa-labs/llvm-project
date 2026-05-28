//===-- TBFFrameLowering.cpp - TBF Frame Information ----------------------===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the TBF implementation of TargetFrameLowering class.
//
//===----------------------------------------------------------------------===//

#include "TBFFrameLowering.h"
#include "TBFSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"

using namespace llvm;

bool TBFFrameLowering::hasFPImpl(const MachineFunction &MF) const { return true; }

void TBFFrameLowering::emitPrologue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {
  const TBFSubtarget& Subtarget = MF.getSubtarget<TBFSubtarget>();
  if (!Subtarget.getHasDynamicFrames()) {
    return;
  }

  MachineBasicBlock::iterator MBBI = MBB.begin();
  MachineFrameInfo &MFI = MF.getFrameInfo();
  int NumBytes = (int)MFI.getStackSize();

  if (MBBI != MBB.end()) {
    DebugLoc Dl = MBBI->getDebugLoc();
    const TBFInstrInfo &TII =
        *static_cast<const TBFInstrInfo *>(MF.getSubtarget().getInstrInfo());

    if (Subtarget.isDynamicFramesV1())
      NumBytes = -NumBytes;
    else if (NumBytes <= FrameSize && !Subtarget.getOptimizeStackSpace())
      // In V3, we don't bump if the number of bytes is less than the default
      // frame size.
      return;
    else
      NumBytes -= FrameSize;

    if (NumBytes)
      BuildMI(MBB, MBBI, Dl, TII.get(TBF::ADD_ri), TBF::R10)
          .addReg(TBF::R10)
          .addImm(NumBytes);
  }
}

void TBFFrameLowering::emitEpilogue(MachineFunction &MF,
                                    MachineBasicBlock &MBB) const {}

void TBFFrameLowering::determineCalleeSaves(MachineFunction &MF,
                                            BitVector &SavedRegs,
                                            RegScavenger *RS) const {
  TargetFrameLowering::determineCalleeSaves(MF, SavedRegs, RS);
  SavedRegs.reset(TBF::R6);
  SavedRegs.reset(TBF::R7);
  SavedRegs.reset(TBF::R8);
  SavedRegs.reset(TBF::R9);
}
