add_llvm_component_library(LLVMOR1KDesc
  OR1KAsmBackend.cpp
  OR1KELFObjectWriter.cpp
  OR1KMCAsmInfo.cpp
  OR1KMCCodeEmitter.cpp
  OR1KMCTargetDesc.cpp
  OR1KInstPrinter.cpp

  LINK_COMPONENTS
  MC
  Support
  OR1KInfo

  ADD_TO_COMPONENT
  OR1K
  )
