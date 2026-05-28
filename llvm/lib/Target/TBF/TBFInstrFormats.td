//===-- TBFInstrFormats.td - TBF Instruction Formats -------*- tablegen -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class TBFOpClass<bits<3> val> {
  bits<3> Value = val;
}

def SBF_LD    : TBFOpClass<0x0>;
def SBF_LDX   : TBFOpClass<0x1>;
def SBF_ST    : TBFOpClass<0x2>;
def SBF_STX   : TBFOpClass<0x3>;
def SBF_ALU   : TBFOpClass<0x4>;
def SBF_JMP   : TBFOpClass<0x5>;
def SBF_PQR   : TBFOpClass<0x6>;
def SBF_JMP32 : TBFOpClass<0x6>;
def SBF_ALU64 : TBFOpClass<0x7>;

def SBF_LD_V2 : TBFOpClass<0x4>;
def SBF_ST_V2 : TBFOpClass<0x7>;

class TBFSrcType<bits<1> val> {
  bits<1> Value = val;
}

def SBF_K : TBFSrcType<0x0>;
def SBF_X : TBFSrcType<0x1>;

class TBFArithOp<bits<4> val> {
  bits<4> Value = val;
}

def SBF_ADD  : TBFArithOp<0x0>;
def SBF_SUB  : TBFArithOp<0x1>;
def SBF_MUL  : TBFArithOp<0x2>;
def SBF_DIV  : TBFArithOp<0x3>;
def SBF_OR   : TBFArithOp<0x4>;
def SBF_AND  : TBFArithOp<0x5>;
def SBF_LSH  : TBFArithOp<0x6>;
def SBF_RSH  : TBFArithOp<0x7>;
def SBF_NEG  : TBFArithOp<0x8>;
def SBF_MOD  : TBFArithOp<0x9>;
def SBF_XOR  : TBFArithOp<0xa>;
def SBF_MOV  : TBFArithOp<0xb>;
def SBF_ARSH : TBFArithOp<0xc>;
def SBF_END  : TBFArithOp<0xd>;
def SBF_SDIV : TBFArithOp<0xe>;
def SBF_HOR  : TBFArithOp<0xf>;

def PQR_UHMUL : TBFArithOp<0x2>;
def PQR_UDIV  : TBFArithOp<0x4>;
def PQR_UREM  : TBFArithOp<0x6>;
def PQR_LMUL  : TBFArithOp<0x8>;
def PQR_SHMUL : TBFArithOp<0xa>;
def PQR_SDIV  : TBFArithOp<0xc>;
def PQR_SREM  : TBFArithOp<0xe>;

def SBF_XCHG    : TBFArithOp<0xe>;
def SBF_CMPXCHG : TBFArithOp<0xf>;

class TBFEndDir<bits<1> val> {
  bits<1> Value = val;
}

def SBF_TO_LE : TBFSrcType<0x0>;
def SBF_TO_BE : TBFSrcType<0x1>;

class TBFJumpOp<bits<4> val> {
  bits<4> Value = val;
}

def SBF_JA      : TBFJumpOp<0x0>;
def SBF_JEQ     : TBFJumpOp<0x1>;
def SBF_JGT     : TBFJumpOp<0x2>;
def SBF_JGE     : TBFJumpOp<0x3>;
def SBF_JNE     : TBFJumpOp<0x5>;
def SBF_JSGT    : TBFJumpOp<0x6>;
def SBF_JSGE    : TBFJumpOp<0x7>;
def SBF_CALL    : TBFJumpOp<0x8>;
def SBF_EXIT    : TBFJumpOp<0x9>;
def SBF_JLT     : TBFJumpOp<0xa>;
def SBF_JLE     : TBFJumpOp<0xb>;
def SBF_JSLT    : TBFJumpOp<0xc>;
def SBF_JSLE    : TBFJumpOp<0xd>;
def SBF_SYSCALL : TBFJumpOp<0x9>;

class TBFWidthModifer<bits<2> val> {
  bits<2> Value = val;
}

def SBF_W  : TBFWidthModifer<0x0>;
def SBF_H  : TBFWidthModifer<0x1>;
def SBF_B  : TBFWidthModifer<0x2>;
def SBF_DW : TBFWidthModifer<0x3>;

class TBFSizeModifier<bits<4> val> {
  bits<4> Value = val;
}

def SBF_B_V2  : TBFSizeModifier<0x2>;
def SBF_H_V2  : TBFSizeModifier<0x3>;
def SBF_W_V2  : TBFSizeModifier<0x8>;
def SBF_DW_V2 : TBFSizeModifier<0x9>;

class TBFModeModifer<bits<3> val> {
  bits<3> Value = val;
}

def SBF_IMM  : TBFModeModifer<0x0>;
def SBF_ABS  : TBFModeModifer<0x1>;
def SBF_IND  : TBFModeModifer<0x2>;
def SBF_MEM  : TBFModeModifer<0x3>;
def SBF_ATOMIC : TBFModeModifer<0x6>;

class TBFAtomicFlag<bits<4> val> {
  bits<4> Value = val;
}

def SBF_FETCH : TBFAtomicFlag<0x1>;

class InstSBF<dag outs, dag ins, string asmstr, list<dag> pattern>
  : Instruction {
  field bits<64> Inst;
  field bits<64> SoftFail = 0;
  let Size = 8;

  let Namespace = "TBF";
  let DecoderNamespace = "TBF";

  TBFOpClass TBFClass;
  let Inst{58-56} = TBFClass.Value;

  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmstr;
  let Pattern = pattern;
}

// Pseudo instructions
class Pseudo<dag outs, dag ins, string asmstr, list<dag> pattern>
  : InstSBF<outs, ins, asmstr, pattern> {
  let Inst{63-0} = 0;
  let isPseudo = 1;
}
