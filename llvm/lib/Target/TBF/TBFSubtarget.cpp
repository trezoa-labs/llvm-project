//===-- TBFSubtarget.cpp - TBF Subtarget Information ----------------------===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the TBF specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "TBFSubtarget.h"
#include "TBF.h"
#include "TBFTargetMachine.h"
#include "GISel/TBFCallLowering.h"
#include "GISel/TBFLegalizerInfo.h"
#include "GISel/TBFRegisterBankInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/TargetParser/Host.h"
#include "TargetInfo/TBFTargetInfo.h"

using namespace llvm;

#define DEBUG_TYPE "tbf-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "TBFGenSubtargetInfo.inc"

void TBFSubtarget::trezoaanchor() {}

TBFSubtarget &TBFSubtarget::initializeSubtargetDependencies(const Triple &TT,
                                                            StringRef CPU,
                                                            StringRef FS) {
  initializeEnvironment(TT);
  initSubtargetFeatures(CPU, FS);
  InstrInfo.initializeTargetFeatures(HasExplicitSignExt, NewMemEncoding);
  return *this;
}

void TBFSubtarget::initializeEnvironment(const Triple &TT) {
  assert(TT.getArch() == Triple::tbf && "expected Triple::tbf");
  UseDwarfRIS = false;

  // New TBF features
  HasDynamicFrames = false;
  DisableNeg = false;
  ReverseSubImm = false;
  NoLddw = false;
  CallxRegSrc = false;
  CallxRegDst = false;
  HasPqrClass = false;
  HasAlu32 = false;
  HasExplicitSignExt = false;
  NewMemEncoding = false;
  HasStaticSyscalls = false;
  IsAbiV2 = false;
  HasJmp32 = false;
  HasDynamicFramesV3 = false;
}

void TBFSubtarget::initSubtargetFeatures(StringRef CPU, StringRef FS) {
  ParseSubtargetFeatures(CPU, /*TuneCPU*/ CPU, FS);
}

TBFSubtarget::TBFSubtarget(const Triple &TT, const std::string &CPU,
                           const std::string &FS, const TargetMachine &TM)
    : TBFGenSubtargetInfo(TT, cpuFromSubArch(TT, CPU), /*TuneCPU*/ cpuFromSubArch(TT, CPU), FS), InstrInfo(),
      FrameLowering(initializeSubtargetDependencies(TT, cpuFromSubArch(TT, CPU), FS)),
      TLInfo(TM, *this) {
  assert(TT.getArch() == Triple::tbf && "expected Triple::tbf");

  CallLoweringInfo.reset(new TBFCallLowering(*getTargetLowering()));
  Legalizer.reset(new TBFLegalizerInfo(*this));
  auto *RBI = new TBFRegisterBankInfo(*getRegisterInfo());
  RegBankInfo.reset(RBI);

  InstSelector.reset(createTBFInstructionSelector(
      *static_cast<const TBFTargetMachine *>(&TM), *this, *RBI));
}
