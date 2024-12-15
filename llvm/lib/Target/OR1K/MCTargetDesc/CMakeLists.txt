add_llvm_component_library(LLVMOR1KDesc
  OR1KMCTargetDesc.cpp

  LINK_COMPONENTS
  MC
  Support
  OR1KInfo

  ADD_TO_COMPONENT
  OR1K
  )
