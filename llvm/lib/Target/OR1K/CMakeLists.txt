add_llvm_component_group(OR1K)

add_llvm_target(OR1KCodeGen
  OR1KTargetMachine.cpp

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
