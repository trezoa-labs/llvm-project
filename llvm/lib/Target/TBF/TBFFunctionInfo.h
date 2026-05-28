//=- TBFFunctionInfo.h - TBF machine function info -*- C++ -*-=//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares TBF-specific per-machine-function information.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_TBFFUNCTIONINFO_H
#define LLVM_TBFFUNCTIONINFO_H

#include "TBFSubtarget.h"
#include <unordered_set>

namespace llvm {

class TBFFunctionInfo final : public MachineFunctionInfo {
  std::unordered_set<int> frameIndexes;

public:
  TBFFunctionInfo(const Function &F, const TBFSubtarget *STI){};

  void storeFrameIndexArgument(int FI);
  bool containsFrameIndex(int FI) const;
};
} // namespace llvm

#endif // LLVM_TBFFUNCTIONINFO_H