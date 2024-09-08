; RUN: llc -march=openrisc < %s | FileCheck %s

@global_var = global i32 3

define i32 @return_global() {
; CHECK: hi(global_var)
; CHECK: lo(global_var)
  %val = load i32, i32* @global_var
  ret i32 %val
}