add_llvm_component_library(LLVMOpenRiscDesc
  OpenRiscAsmBackend.cpp
  OpenRiscELFObjectWriter.cpp
  OpenRiscInstPrinter.cpp
  OpenRiscMCAsmInfo.cpp
  OpenRiscMCCodeEmitter.cpp
  OpenRiscMCExpr.cpp
  OpenRiscMCTargetDesc.cpp
  OpenRiscTargetStreamer.cpp

  LINK_COMPONENTS
  MC
  Support
  OpenRiscInfo

  ADD_TO_COMPONENT
  OpenRisc
 )
