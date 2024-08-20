add_llvm_component_library(LLVMOpenRiscInfo
  OpenRiscTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  OpenRisc
  )
