add_llvm_component_library(LLVMTBFDisassembler
  TBFDisassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  MC
  TBFInfo
  Support

  ADD_TO_COMPONENT
  TBF
)

