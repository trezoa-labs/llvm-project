//===-- TBFMCAsmInfo.h - TBF asm properties -------------------*- C++ -*--====//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the TBFMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_MCTARGETDESC_TBFMCASMINFO_H
#define LLVM_LIB_TARGET_TBF_MCTARGETDESC_TBFMCASMINFO_H

#include "llvm/MC/MCAsmInfo.h"
#include "llvm/TargetParser/Triple.h"

namespace llvm {

// TODO: This should likely be subclassing MCAsmInfoELF.
class TBFMCAsmInfo : public MCAsmInfo {
public:
  explicit TBFMCAsmInfo(const Triple &TT, const MCTargetOptions &Options);

  void setDwarfUsesRelocationsAcrossSections(bool enable) {
    DwarfUsesRelocationsAcrossSections = enable;
  }

  void setSupportsDebugInformation(bool enable) {
    SupportsDebugInformation = enable;
  }
};

}
#endif
