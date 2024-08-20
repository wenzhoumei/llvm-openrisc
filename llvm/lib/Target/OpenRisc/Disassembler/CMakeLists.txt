add_llvm_component_library(LLVMOpenRiscDisassembler
  OpenRiscDisassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  Support
  OpenRiscInfo

  ADD_TO_COMPONENT
  OpenRisc
  )
