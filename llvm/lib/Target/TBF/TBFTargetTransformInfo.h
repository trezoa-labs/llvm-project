//===------ TBFTargetTransformInfo.h - TBF specific TTI ---------*- C++ -*-===//
//
// Part of the LLVM Trezoa, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file uses the target's specific information to
// provide more precise answers to certain TTI queries, while letting the
// target independent and default TTI implementations handle the rest.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_TBF_TBFTARGETTRANSFORMINFO_H
#define LLVM_LIB_TARGET_TBF_TBFTARGETTRANSFORMINFO_H

#include "TBFTargetMachine.h"
#include "llvm/Analysis/TargetTransformInfo.h"
#include "llvm/CodeGen/BasicTTIImpl.h"
#include "llvm/Transforms/Utils/ScalarEvolutionExpander.h"

namespace llvm {
class TBFTTIImpl : public BasicTTIImplBase<TBFTTIImpl> {
  typedef BasicTTIImplBase<TBFTTIImpl> BaseT;
  typedef TargetTransformInfo TTI;
  friend BaseT;

  const TBFSubtarget *ST;
  const TBFTargetLowering *TLI;

  const TBFSubtarget *getST() const { return ST; }
  const TBFTargetLowering *getTLI() const { return TLI; }

public:
  explicit TBFTTIImpl(const TBFTargetMachine *TM, const Function &F)
      : BaseT(TM, F.getParent()->getDataLayout()), ST(TM->getSubtargetImpl(F)),
        TLI(ST->getTargetLowering()) {}

  int getIntImmCost(const APInt &Imm, Type *Ty, TTI::TargetCostKind CostKind) {
    if (Imm.getBitWidth() <= 64 && isInt<32>(Imm.getSExtValue()))
      return TTI::TCC_Free;

    return TTI::TCC_Basic;
  }

  bool shouldBuildLookupTables() const {
    return true;
  }

  bool shouldBuildRelLookupTables() const {
    // Relational lookup tables are not working for TBF, since the offset
    // calculation is not implemented.
    return false;
  }

  InstructionCost getCmpSelInstrCost(
      unsigned Opcode, Type *ValTy, Type *CondTy, CmpInst::Predicate VecPred,
      TTI::TargetCostKind CostKind,
      TTI::OperandValueInfo Op1Info = {TTI::OK_AnyValue, TTI::OP_None},
      TTI::OperandValueInfo Op2Info = {TTI::OK_AnyValue, TTI::OP_None},
      const llvm::Instruction *I = nullptr) {
    if (Opcode == Instruction::Select)
      return SCEVCheapExpansionBudget.getValue();

    return BaseT::getCmpSelInstrCost(Opcode, ValTy, CondTy, VecPred, CostKind,
                                     Op1Info, Op2Info, I);
  }

  InstructionCost getArithmeticInstrCost(
      unsigned Opcode, Type *Ty, TTI::TargetCostKind CostKind,
      TTI::OperandValueInfo Op1Info = {TTI::OK_AnyValue, TTI::OP_None},
      TTI::OperandValueInfo Op2Info = {TTI::OK_AnyValue, TTI::OP_None},
    ArrayRef<const Value *> Args = {}, const Instruction *CxtI = nullptr) {
    int ISDOpcode = TLI->InstructionOpcodeToISD(Opcode);
    
    if ((ISDOpcode == ISD::ADD && CostKind == TTI::TCK_RecipThroughput) ||
        !Ty->isIntOrPtrTy())
      return SCEVCheapExpansionBudget.getValue() + 1;

    return BaseT::getArithmeticInstrCost(Opcode, Ty, CostKind, Op1Info,
                                         Op2Info);
  }

  TTI::MemCmpExpansionOptions enableMemCmpExpansion(bool OptSize,
                                                    bool IsZeroCmp) const {
    TTI::MemCmpExpansionOptions Options;
    Options.LoadSizes = {8, 4, 2, 1};
    Options.MaxNumLoads = TLI->getMaxExpandSizeMemcmp(OptSize);
    Options.AllowOverlappingLoads = true;
    return Options;
  }

};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_TBF_TBFTARGETTRANSFORMINFO_H
