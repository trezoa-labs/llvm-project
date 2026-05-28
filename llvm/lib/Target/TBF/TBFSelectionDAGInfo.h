//===-- TBFSelectionDAGInfo.h - TBF SelectionDAG Info -----------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the TBF subclass for SelectionDAGTargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_TBFSELECTIONDAGINFO_H
#define LLVM_LIB_TARGET_TBF_TBFSELECTIONDAGINFO_H

#include "llvm/CodeGen/SelectionDAGTargetInfo.h"

namespace llvm {

class TBFSelectionDAGInfo : public SelectionDAGTargetInfo {
public:
  TBFSelectionDAGInfo() {}

  unsigned getCommonMaxStoresPerMemFunc() const {
    return 4;
  }
};

}

#endif
