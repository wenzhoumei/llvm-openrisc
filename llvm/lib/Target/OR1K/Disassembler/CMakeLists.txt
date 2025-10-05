add_llvm_component_library(LLVMOR1KDisassembler
  OR1KDisassembler.cpp

  LINK_COMPONENTS
  MCDisassembler
  Support
  OR1KDesc
  OR1KInfo

  ADD_TO_COMPONENT
  OR1K
  )
