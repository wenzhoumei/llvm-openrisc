add_llvm_component_group(OR1K)

set(LLVM_TARGET_DEFINITIONS OR1K.td)

tablegen(LLVM OR1KGenMCCodeEmitter.inc -gen-emitter)
tablegen(LLVM OR1KGenInstrInfo.inc -gen-instr-info)
tablegen(LLVM OR1KGenRegisterInfo.inc -gen-register-info)
tablegen(LLVM OR1KGenAsmWriter.inc -gen-asm-writer)
tablegen(LLVM OR1KGenSubtargetInfo.inc -gen-subtarget)
tablegen(LLVM OR1KGenAsmMatcher.inc -gen-asm-matcher)

add_public_tablegen_target(OR1KCommonTableGen)

add_llvm_target(OR1KCodeGen
  OR1KTargetMachine.cpp
  OR1KSubtarget.cpp

  LINK_COMPONENTS
  CodeGen
  Core
  Support
  Target
  OR1KInfo

  ADD_TO_COMPONENT
  OR1K
  )

add_subdirectory(MCTargetDesc)
add_subdirectory(TargetInfo)
add_subdirectory(AsmParser)
