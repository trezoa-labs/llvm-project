//===- TBFLegalizerInfo.h ----------------------------------------*- C++ -*-==//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// \file
/// This file declares the targeting of the Machinelegalizer class for TBF
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_GISEL_TBFMACHINELEGALIZER_H
#define LLVM_LIB_TARGET_TBF_GISEL_TBFMACHINELEGALIZER_H

#include "llvm/CodeGen/GlobalISel/LegalizerInfo.h"

namespace llvm {

class TBFSubtarget;

/// This class provides the information for the TBF target legalizer for
/// GlobalISel.
class TBFLegalizerInfo : public LegalizerInfo {
public:
  TBFLegalizerInfo(const TBFSubtarget &ST);
};
} // namespace llvm
#endif
