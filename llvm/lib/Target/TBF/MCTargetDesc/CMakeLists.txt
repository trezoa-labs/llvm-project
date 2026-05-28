add_llvm_component_library(LLVMTBFDesc
  TBFMCTargetDesc.cpp
  TBFAsmBackend.cpp
  TBFInstPrinter.cpp
  TBFMCAsmInfo.cpp
  TBFMCCodeEmitter.cpp
  TBFELFObjectWriter.cpp

  LINK_COMPONENTS
  MC
  TBFInfo
  Support
  TargetParser

  ADD_TO_COMPONENT
  TBF
  )
