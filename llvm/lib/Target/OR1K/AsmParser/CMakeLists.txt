add_llvm_component_library(LLVMOR1KAsmParser
  OR1KAsmParser.cpp

  LINK_COMPONENTS
  MC
  MCParser
  Support
  OR1KDesc
  OR1KInfo

  ADD_TO_COMPONENT
  OR1K
  )
