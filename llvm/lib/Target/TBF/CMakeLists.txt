add_llvm_component_group(TBF)

set(LLVM_TARGET_DEFINITIONS TBF.td)

tablegen(LLVM TBFGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM TBFGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM TBFGenCallingConv.inc -gen-callingconv)
tablegen(LLVM TBFGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM TBFGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM TBFGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM TBFGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM TBFGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM TBFGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM TBFGenGlobalISel.inc -gen-global-isel)
tablegen(LLVM TBFGenRegisterBank.inc -gen-register-bank)

add_public_tablegen_target(TBFCommonTableGen)

add_llvm_target(TBFCodeGen
  GISel/TBFCallLowering.cpp
  GISel/TBFInstructionSelector.cpp
  GISel/TBFRegisterBankInfo.cpp
  GISel/TBFLegalizerInfo.cpp
  TBFAbstractMemberAccess.cpp
  TBFAsmPrinter.cpp
  TBFCheckAndAdjustIR.cpp
  TBFFrameLowering.cpp
  TBFFunctionInfo.cpp
  TBFInstrInfo.cpp
  TBFIRPeephole.cpp
  TBFISelDAGToDAG.cpp
  TBFISelLowering.cpp
  TBFMCInstLower.cpp
  TBFPreserveDIType.cpp
  TBFRegisterInfo.cpp
  TBFSubtarget.cpp
  TBFTargetMachine.cpp
  TBFMIPeephole.cpp
  TBFMIChecking.cpp
  TBFMISimplifyPatchable.cpp
  BTFDebug.cpp

  LINK_COMPONENTS
  Analysis
  AsmPrinter
  CodeGen
  CodeGenTypes
  Core
  GlobalISel
  MC
  TBFDesc
  TBFInfo
  IPO
  Passes
  Scalar
  SelectionDAG
  Support
  Target
  TargetParser
  TransformUtils

  ADD_TO_COMPONENT
  TBF
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
