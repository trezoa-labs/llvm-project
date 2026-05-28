add_llvm_component_library(LLVMTBFInfo
  TBFTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  TBF
  )
