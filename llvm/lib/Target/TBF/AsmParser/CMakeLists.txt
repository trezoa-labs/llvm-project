add_llvm_component_library(LLVMTBFAsmParser
  TBFAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  TBFDesc
  TBFInfo
  Support

  ADD_TO_COMPONENT
  TBF
  )
