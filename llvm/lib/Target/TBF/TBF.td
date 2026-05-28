//===-- TBF.td - Describe the TBF Target Machine -----------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

include "TBFRegisterInfo.td"
include "TBFCallingConv.td"
include "TBFInstrInfo.td"
include "GISel/TBFRegisterBanks.td"

def TBFInstrInfo : InstrInfo;
//===----------------------------------------------------------------------===//
// Assembly printer
//===----------------------------------------------------------------------===//

def TBFAsmWriter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
  int Variant = 0;
  bit isMCAsmWriter = 1;
}

//===----------------------------------------------------------------------===//
// Assembly parser
//===----------------------------------------------------------------------===//

def TBFAsmParser : AsmParser;

def TBFAsmParserVariant : AsmParserVariant {
  int Variant = 0;
  string Name = "tbf";
  string BreakCharacters = ".";
}

//===----------------------------------------------------------------------===//
// Target Declaration
//===----------------------------------------------------------------------===//

def TBF : Target {
  let InstructionSet = TBFInstrInfo;
  let AssemblyWriters = [TBFAsmWriter];
  let AssemblyParsers = [TBFAsmParser];
  let AssemblyParserVariants = [TBFAsmParserVariant];
}
