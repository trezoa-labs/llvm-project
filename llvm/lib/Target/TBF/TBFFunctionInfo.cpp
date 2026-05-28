//=- TBFFunctionInfo.cpp - TBF Machine Function Info ---------=//

//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file implements TBF-specific per-machine-function
/// information.
///
//===----------------------------------------------------------------------===//

#include "TBFFunctionInfo.h"

namespace llvm {

void TBFFunctionInfo::storeFrameIndexArgument(int FI) {
  frameIndexes.insert(FI);
}

bool TBFFunctionInfo::containsFrameIndex(int FI) const {
  return frameIndexes.find(FI) != frameIndexes.end();
}

} // namespace llvm