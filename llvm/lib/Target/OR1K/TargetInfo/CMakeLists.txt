add_llvm_component_library(LLVMOR1KInfo
  OR1KTargetInfo.cpp

  LINK_COMPONENTS
  MC
  Support

  ADD_TO_COMPONENT
  OR1K
  )
