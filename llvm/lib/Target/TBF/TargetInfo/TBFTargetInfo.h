//===-- TBFTargetInfo.h - TBF Target Implementation -------------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "llvm/TargetParser/Triple.h"

#ifndef LLVM_LIB_TARGET_TBF_TARGETINFO_TBFTARGETINFO_H
#define LLVM_LIB_TARGET_TBF_TARGETINFO_TBFTARGETINFO_H

namespace llvm {

class Target;

Target &getTheTBFXTarget();
std::string cpuFromSubArch(const Triple &TT, const std::string &CPU);
} // namespace llvm

#endif // LLVM_LIB_TARGET_TBF_TARGETINFO_TBFTARGETINFO_H
