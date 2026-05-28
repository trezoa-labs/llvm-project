//===- TBFLegalizerInfo.h ----------------------------------------*- C++ -*-==//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file implements the targeting of the Machinelegalizer class for TBF
//===----------------------------------------------------------------------===//

#include "TBFLegalizerInfo.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "TBF-legalinfo"

using namespace llvm;
using namespace LegalizeActions;

TBFLegalizerInfo::TBFLegalizerInfo(const TBFSubtarget &ST) {
  getLegacyLegalizerInfo().computeTables();
}
