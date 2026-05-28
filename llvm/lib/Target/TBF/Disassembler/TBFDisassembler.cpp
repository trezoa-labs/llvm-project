//===- TBFDisassembler.cpp - Disassembler for TBF ---------------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file is part of the TBF Disassembler.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/TBFMCTargetDesc.h"
#include "TargetInfo/TBFTargetInfo.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/MathExtras.h"
#include <cstdint>

using namespace llvm;

#define DEBUG_TYPE "tbf-disassembler"

typedef MCDisassembler::DecodeStatus DecodeStatus;

namespace {

/// A disassembler class for TBF.
class TBFDisassembler : public MCDisassembler {
public:
  enum SBF_CLASS {
    SBF_LD = 0x0,
    SBF_LDX = 0x1,
    SBF_ST = 0x2,
    SBF_STX = 0x3,
    SBF_ALU = 0x4,
    SBF_JMP = 0x5,
    SBF_PQR_OR_JMP32 = 0x6,
    SBF_ALU64 = 0x7
  };

  enum SBF_SIZE { SBF_W = 0x0, SBF_H = 0x1, SBF_B = 0x2, SBF_DW = 0x3 };

  enum SBF_MODE {
    SBF_IMM = 0x0,
    SBF_ABS = 0x1,
    SBF_IND = 0x2,
    SBF_MEM = 0x3,
    SBF_LEN = 0x4,
    SBF_MSH = 0x5,
    SBF_ATOMIC = 0x6
  };

  TBFDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx)
      : MCDisassembler(STI, Ctx) {}
  ~TBFDisassembler() override = default;

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;

  uint8_t getInstClass(uint64_t Inst) const { return (Inst >> 56) & 0x7; };
  uint8_t getInstSize(uint64_t Inst) const { return (Inst >> 59) & 0x3; };
  uint8_t getInstMode(uint64_t Inst) const { return (Inst >> 61) & 0x7; };
  bool isMov32(uint64_t Inst) const { return (Inst >> 56) == 0xb4; }
  bool isNewMem(uint64_t Inst) const;
  bool isSyscallOrExit(uint64_t Inst) const { return (Inst >> 56) == 0x95; }
  bool isAlu32NewLoadStoreReg(uint64_t Inst) const;
};

} // end anonymous namespace

static MCDisassembler *createTBFDisassembler(const Target &T,
                                             const MCSubtargetInfo &STI,
                                             MCContext &Ctx) {
  return new TBFDisassembler(STI, Ctx);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeTBFDisassembler() {
  // Register the disassembler.
  TargetRegistry::RegisterMCDisassembler(getTheTBFXTarget(),
                                         createTBFDisassembler);
}

static const unsigned GPRDecoderTable[] = {
    TBF::R0, TBF::R1, TBF::R2, TBF::R3, TBF::R4,  TBF::R5,
    TBF::R6, TBF::R7, TBF::R8, TBF::R9, TBF::R10
};

static DecodeStatus DecodeGPRRegisterClass(MCInst &Inst, unsigned RegNo,
                                           uint64_t /*Address*/,
                                           const MCDisassembler * /*Decoder*/) {
  if (RegNo > 11)
    return MCDisassembler::Fail;

  unsigned Reg = GPRDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static const unsigned GPR32DecoderTable[] = {
    TBF::W0, TBF::W1, TBF::W2, TBF::W3, TBF::W4,  TBF::W5,
    TBF::W6, TBF::W7, TBF::W8, TBF::W9, TBF::W10
};

static DecodeStatus
DecodeGPR32RegisterClass(MCInst &Inst, unsigned RegNo, uint64_t /*Address*/,
                         const MCDisassembler * /*Decoder*/) {
  if (RegNo > 10)
    return MCDisassembler::Fail;

  unsigned Reg = GPR32DecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}

static DecodeStatus decodeMemoryOpValue(MCInst &Inst, unsigned Insn,
                                        uint64_t Address,
                                        const MCDisassembler *Decoder) {
  unsigned Register = (Insn >> 16) & 0xf;
  if (Register > 10)
    return MCDisassembler::Fail;

  Inst.addOperand(MCOperand::createReg(GPRDecoderTable[Register]));
  unsigned Offset = (Insn & 0xffff);
  Inst.addOperand(MCOperand::createImm(SignExtend32<16>(Offset)));

  return MCDisassembler::Success;
}

#include "TBFGenDisassemblerTables.inc"
static DecodeStatus readInstruction64(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint64_t &Insn,
                                      bool IsLittleEndian) {
  uint64_t Lo, Hi;

  if (Bytes.size() < 8) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  Size = 8;
  if (IsLittleEndian) {
    Hi =
        (Bytes[0] << 24) | (Bytes[1] << 16) | (Bytes[2] << 0) | (Bytes[3] << 8);
    Lo =
        (Bytes[4] << 0) | (Bytes[5] << 8) | (Bytes[6] << 16) | (Bytes[7] << 24);
  } else {
    Hi = (Bytes[0] << 24) | ((Bytes[1] & 0x0F) << 20) |
         ((Bytes[1] & 0xF0) << 12) | (Bytes[2] << 8) | (Bytes[3] << 0);
    Lo =
        (Bytes[4] << 24) | (Bytes[5] << 16) | (Bytes[6] << 8) | (Bytes[7] << 0);
  }
  Insn = Make_64(Hi, Lo);

  return MCDisassembler::Success;
}

bool TBFDisassembler::isNewMem(uint64_t Inst) const {
  uint8_t OpCode = Inst >> 56;

  uint8_t LSB = OpCode & 0xf;
  if (LSB != 0x7 && LSB != 0xc && LSB != 0xf)
    return false;

  uint8_t MSB = OpCode >> 4;

  return MSB == 0x2 || MSB == 0x3 || MSB == 0x8 || MSB == 0x9;
}

bool TBFDisassembler::isAlu32NewLoadStoreReg(uint64_t Inst) const {
  bool IsNotDw = (Inst >> 60) != 0x9;
  bool IsNotStoreImm = (Inst >> 56 & 0xf) != 0x7;
  return isNewMem(Inst) && IsNotDw && IsNotStoreImm &&
      STI.hasFeature(TBF::FeatureNewMemEncoding) &&
      STI.hasFeature(TBF::ALU32);
}

DecodeStatus TBFDisassembler::getInstruction(MCInst &Instr, uint64_t &Size,
                                             ArrayRef<uint8_t> Bytes,
                                             uint64_t Address,
                                             raw_ostream &CStream) const {
  bool IsLittleEndian = getContext().getAsmInfo()->isLittleEndian();
  uint64_t Insn, Hi;
  DecodeStatus Result;

  Result = readInstruction64(Bytes, Address, Size, Insn, IsLittleEndian);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  uint8_t InstClass = getInstClass(Insn);
  uint8_t InstMode = getInstMode(Insn);

  if ((InstClass == SBF_LDX || InstClass == SBF_STX) &&
      getInstSize(Insn) != SBF_DW &&
      (InstMode == SBF_MEM || InstMode == SBF_ATOMIC) &&
      STI.hasFeature(TBF::ALU32))
    Result = decodeInstruction(DecoderTableTBFALU3264, Instr, Insn, Address,
                               this, STI);
  else if (isMov32(Insn) && !STI.hasFeature(TBF::ALU32) &&
           STI.hasFeature(TBF::FeatureDisableLddw))
    Result =
        decodeInstruction(DecoderTableTBFv264, Instr, Insn, Address, this, STI);
  else if (isAlu32NewLoadStoreReg(Insn)) {
    Result =
        decodeInstruction(DecoderTableTBFALU32MEMv264,
                          Instr, Insn, Address, this, STI);
  }
  else if (isNewMem(Insn) && STI.hasFeature(TBF::FeatureNewMemEncoding)) {
    Result =
        decodeInstruction(DecoderTableTBFv264,
                          Instr, Insn, Address, this, STI);
  } else if (InstClass == SBF_PQR_OR_JMP32 && !STI.hasFeature(TBF::FeaturePqrInstr)) {
    Result = decodeInstruction(DecoderTableTBFv364,
                               Instr, Insn, Address, this, STI);
  }
  else
    Result =
        decodeInstruction(DecoderTableTBF64, Instr, Insn, Address, this, STI);

  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;

  switch (Instr.getOpcode()) {
  case TBF::LD_imm64:
  case TBF::LD_pseudo: {
    if (Bytes.size() < 16) {
      Size = 0;
      return MCDisassembler::Fail;
    }
    Size = 16;
    if (IsLittleEndian)
      Hi = (Bytes[12] << 0) | (Bytes[13] << 8) | (Bytes[14] << 16) |
           (Bytes[15] << 24);
    else
      Hi = (Bytes[12] << 24) | (Bytes[13] << 16) | (Bytes[14] << 8) |
           (Bytes[15] << 0);
    auto &Op = Instr.getOperand(1);
    Op.setImm(Make_64(Hi, Op.getImm()));
    break;
  }
  case TBF::JALX: {
    if (STI.hasFeature(TBF::FeatureCallxRegSrc)) {
      Result = decodeInstruction(DecoderTableTBFv264, Instr, Insn, Address,
                                 this, STI);
    } else if (STI.hasFeature(TBF::FeatureCallxRegDst)) {
      Result = decodeInstruction(DecoderTableTBFv364, Instr, Insn, Address,
                                 this, STI);
    }
  }
  }

  return Result;
}

typedef DecodeStatus (*DecodeFunc)(MCInst &MI, unsigned insn, uint64_t Address,
                                   const MCDisassembler *Decoder);
