add_llvm_component_library(LLVMOpenRiscAsmParser
  OpenRiscAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  Support
  OpenRiscDesc
  OpenRiscInfo

  ADD_TO_COMPONENT
  OpenRisc
  )
