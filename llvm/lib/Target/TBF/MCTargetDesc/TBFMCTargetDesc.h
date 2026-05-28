//===-- TBFMCTargetDesc.h - TBF Target Descriptions -------------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides TBF specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_MCTARGETDESC_TBFMCTARGETDESC_H
#define LLVM_LIB_TARGET_TBF_MCTARGETDESC_TBFMCTARGETDESC_H

#include "llvm/Config/config.h"
#include "llvm/MC/MCContext.h"
#include "llvm/Support/DataTypes.h"

#include <memory>

namespace llvm {
class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class Target;

MCCodeEmitter *createTBFMCCodeEmitter(const MCInstrInfo &MCII,
                                      MCContext &Ctx);
MCCodeEmitter *createSBFbeMCCodeEmitter(const MCInstrInfo &MCII,
                                        MCContext &Ctx);

MCAsmBackend *createTBFAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                  const MCRegisterInfo &MRI,
                                  const MCTargetOptions &Options);
MCAsmBackend *createSBFbeAsmBackend(const Target &T, const MCSubtargetInfo &STI,
                                    const MCRegisterInfo &MRI,
                                    const MCTargetOptions &Options);

std::unique_ptr<MCObjectTargetWriter>
createTBFELFObjectWriter(uint8_t OSABI, bool useRelocAbs64);
} // namespace llvm

// Defines symbolic names for TBF registers.  This defines a mapping from
// register name to register number.
//
#define GET_REGINFO_ENUM
#include "TBFGenRegisterInfo.inc"

// Defines symbolic names for the TBF instructions.
//
#define GET_INSTRINFO_ENUM
#define GET_INSTRINFO_MC_HELPER_DECLS
#include "TBFGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "TBFGenSubtargetInfo.inc"

#endif
