//===-- TBFTargetInfo.cpp - TBF Target Implementation ---------------------===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/TBFTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

Target &llvm::getTheTBFXTarget() {
  static Target TheTBFTarget;
  return TheTBFTarget;
}

std::string llvm::cpuFromSubArch(const Triple &TT, const std::string &CPU) {
  std::string CpuType;
  switch (TT.getSubArch()) {
  case Triple::TBFSubArch_v0:
    CpuType = "generic";
    break;
  case Triple::TBFSubArch_v1:
    CpuType = "v1";
    break;
  case Triple::TBFSubArch_v2:
    CpuType = "v2";
    break;
  case Triple::TBFSubArch_v3:
    CpuType = "v3";
    break;
  case Triple::TBFSubArch_v4:
    CpuType = "v4";
    break;
  default:
    break;
  }

  assert((CPU.empty() || CpuType.empty() || CPU == CpuType) &&
         "Subarch type must match CPU type");

  if (!CpuType.empty()) {
    return CpuType;
  }

  return CPU;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTBFTargetInfo() {
  TargetRegistry::RegisterTarget(
      getTheTBFXTarget(), "tbf", "TBF new (little endian)", "TBF",
      [](Triple::ArchType Aarch) { return Aarch == Triple::ArchType::tbf; },
      true);
  RegisterTarget<Triple::tbf, /*HasJIT=*/true> XX(
      getTheTBFXTarget(), "tbf", "TBF new (little endian)", "TBF");
}
