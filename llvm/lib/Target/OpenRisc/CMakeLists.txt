add_llvm_component_group(OpenRisc)

set(LLVM_TARGET_DEFINITIONS OpenRisc.td)

tablegen(LLVM OpenRiscGenAsmMatcher.inc -gen-asm-matcher)
tablegen(LLVM OpenRiscGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM OpenRiscGenCallingConv.inc -gen-callingconv)
tablegen(LLVM OpenRiscGenDAGISel.inc -gen-dag-isel)
tablegen(LLVM OpenRiscGenDisassemblerTables.inc -gen-disassembler)
tablegen(LLVM OpenRiscGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM OpenRiscGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM OpenRiscGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM OpenRiscGenSubtargetInfo.inc -gen-subtarget)

add_public_tablegen_target(OpenRiscCommonTableGen)

add_llvm_target(OpenRiscCodeGen
  OpenRiscAsmPrinter.cpp
  OpenRiscFrameLowering.cpp
  OpenRiscInstrInfo.cpp
  OpenRiscISelDAGToDAG.cpp
  OpenRiscISelLowering.cpp
  OpenRiscRegisterInfo.cpp
  OpenRiscSubtarget.cpp
  OpenRiscTargetMachine.cpp
  OpenRiscUtils.cpp

  LINK_COMPONENTS
  AsmPrinter
  CodeGen
  Core
  MC
  SelectionDAG
  Support
  Target
  OpenRiscDesc
  OpenRiscInfo

  ADD_TO_COMPONENT
  OpenRisc
  )

add_subdirectory(AsmParser)
add_subdirectory(Disassembler)
add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)

