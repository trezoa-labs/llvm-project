//===-- TBFRegisterBankInfo.h -----------------------------------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file declares the targeting of the RegisterBankInfo class for TBF.
///
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_GISEL_TBFREGISTERBANKINFO_H
#define LLVM_LIB_TARGET_TBF_GISEL_TBFREGISTERBANKINFO_H

#include "MCTargetDesc/TBFMCTargetDesc.h"
#include "llvm/CodeGen/RegisterBankInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGBANK_DECLARATIONS
#include "TBFGenRegisterBank.inc"

namespace llvm {
class TargetRegisterInfo;

class TBFGenRegisterBankInfo : public RegisterBankInfo {
protected:
#define GET_TARGET_REGBANK_CLASS
#include "TBFGenRegisterBank.inc"
};

class TBFRegisterBankInfo final : public TBFGenRegisterBankInfo {
public:
  TBFRegisterBankInfo(const TargetRegisterInfo &TRI);
};
} // namespace llvm

#endif
