//===-- TBFRegisterInfo.cpp - TBF Register Information ----------*- C++ -*-===//
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

#include "TBFFunctionInfo.h"
#include "TBFRegisterInfo.h"
#include "TBFSubtarget.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineFunction.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/RegisterScavenging.h"
#include "llvm/CodeGen/TargetFrameLowering.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/IR/DiagnosticInfo.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_REGINFO_TARGET_DESC
#include "TBFGenRegisterInfo.inc"
using namespace llvm;

unsigned TBFRegisterInfo::FrameLength = 4096;

TBFRegisterInfo::TBFRegisterInfo()
    : TBFGenRegisterInfo(TBF::R0) {}

const MCPhysReg *
TBFRegisterInfo::getCalleeSavedRegs(const MachineFunction *MF) const {
  return CSR_SaveList;
}

BitVector TBFRegisterInfo::getReservedRegs(const MachineFunction &MF) const {
  BitVector Reserved(getNumRegs());
  markSuperRegs(Reserved, TBF::W10); // [W|R]10 is read only frame pointer
  return Reserved;
}

static void WarnSize(int Offset, MachineFunction &MF, DebugLoc& DL)
{
  static Function *OldMF = nullptr;
  int MaxOffset = -1 * TBFRegisterInfo::FrameLength;
  if (Offset < MaxOffset) {

    if (&(MF.getFunction()) == OldMF) {
      return;
    }
    OldMF = &(MF.getFunction());

    dbgs() << "Error:";
    if (DL) {
      dbgs() << " ";
      DL.print(dbgs());
    }
    uint64_t StackSize = MF.getFrameInfo().getStackSize();
    dbgs() << " Function " << MF.getFunction().getName()
           << " Stack offset of " << -Offset << " exceeded max offset of "
           << -MaxOffset << " by " << MaxOffset - Offset
           << " bytes, please minimize large stack variables. "
           << "Estimated function frame size: " << StackSize << " bytes."
           << " Exceeding the maximum stack offset may cause "
              "undefined behavior during execution.\n\n";
  }
}

bool TBFRegisterInfo::eliminateFrameIndex(MachineBasicBlock::iterator II,
                                          int SPAdj, unsigned FIOperandNum,
                                          RegScavenger *RS) const {
  assert(SPAdj == 0 && "Unexpected");

  unsigned i = 0;
  MachineInstr &MI = *II;
  MachineBasicBlock &MBB = *MI.getParent();
  MachineFunction &MF = *MBB.getParent();
  DebugLoc DL = MI.getDebugLoc();

  if (!DL)
    /* try harder to get some debug loc */
    for (auto &I : MBB)
      if (I.getDebugLoc()) {
        DL = I.getDebugLoc().getFnDebugLoc();
        break;
      }

  while (!MI.getOperand(i).isFI()) {
    ++i;
    assert(i < MI.getNumOperands() && "Instr doesn't have FrameIndex operand!");
  }

  Register FrameReg = getFrameRegister(MF);
  int FrameIndex = MI.getOperand(i).getIndex();
  const TargetInstrInfo &TII = *MF.getSubtarget().getInstrInfo();

  if (MI.getOpcode() == TBF::MOV_rr) {
    int Offset = resolveInternalFrameIndex(MF, FrameIndex, std::nullopt);

    if (!MF.getSubtarget<TBFSubtarget>().getHasDynamicFrames()) {
      WarnSize(Offset, MF, DL);
    }
    MI.getOperand(i).ChangeToRegister(FrameReg, false);
    Register reg = MI.getOperand(i - 1).getReg();
    BuildMI(MBB, ++II, DL, TII.get(TBF::ADD_ri), reg)
        .addReg(reg)
        .addImm(Offset);
    return false;
  }

  int Offset =
      resolveInternalFrameIndex(MF, FrameIndex, MI.getOperand(i + 1).getImm());


  if (!isInt<32>(Offset))
    llvm_unreachable("bug in frame offset");

  if (!MF.getSubtarget<TBFSubtarget>().getHasDynamicFrames()) {
    WarnSize(Offset, MF, DL);
  }

  if (MI.getOpcode() == TBF::FI_ri) {
    // architecture does not really support FI_ri, replace it with
    //    MOV_rr <target_reg>, frame_reg
    //    ADD_ri <target_reg>, imm
    Register reg = MI.getOperand(i - 1).getReg();

    BuildMI(MBB, ++II, DL, TII.get(TBF::MOV_rr), reg)
        .addReg(FrameReg);
    BuildMI(MBB, II, DL, TII.get(TBF::ADD_ri), reg)
        .addReg(reg)
        .addImm(Offset);

    // Remove FI_ri instruction
    MI.eraseFromParent();
  } else {
    MI.getOperand(i).ChangeToRegister(FrameReg, false);
    MI.getOperand(i + 1).ChangeToImmediate(Offset);
  }
  return false;
}

int TBFRegisterInfo::resolveInternalFrameIndex(
    const llvm::MachineFunction &MF, int FI, std::optional<int64_t> Imm) const {
  const MachineFrameInfo &MFI = MF.getFrameInfo();
  const TBFFunctionInfo *TBFFuncInfo = MF.getInfo<TBFFunctionInfo>();
  int Offset = MFI.getObjectOffset(FI);
  const TBFSubtarget & SubTarget = MF.getSubtarget<TBFSubtarget>();
  uint64_t StackSize = MFI.getStackSize();

  if (!SubTarget.getHasDynamicFrames() &&
      TBFFuncInfo->containsFrameIndex(FI)) {
    Offset = TBFRegisterInfo::FrameLength - Offset;
    if (static_cast<uint64_t>(Offset) < StackSize) {
      dbgs() << "Error: A function call in method "
             << MF.getFunction().getName()
             << " overwrites values in the frame. Please, decrease stack usage "
             << "or remove parameters from the call. "
             << "The function call may cause undefined behavior "
                "during execution.\n\n";
    }
    return -Offset;
  }

  if (SubTarget.getHasDynamicFrames() &&
      TBFFuncInfo->containsFrameIndex(FI)) {
    if (SubTarget.isDynamicFramesV1())
      return -Offset;

    return Offset;
  }

  Offset += Imm.value_or(0);

  if (SubTarget.getHasDynamicFrames()) {
    if (SubTarget.isDynamicFramesV1())
      return Offset + static_cast<int>(StackSize);

    if (SubTarget.getOptimizeStackSpace())
      return -(Offset + static_cast<int>(StackSize));

    return -(Offset + std::max(static_cast<int>(StackSize), static_cast<int>(FrameLength)));
  }

  return Offset;
}

Register TBFRegisterInfo::getFrameRegister(const MachineFunction &MF) const {
  return TBF::R10;
}
