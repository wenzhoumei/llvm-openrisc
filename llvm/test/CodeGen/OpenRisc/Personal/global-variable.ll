; RUN: llc -march=openrisc < %s | FileCheck %s

@global_var = global i32 0

define void @return_global() {
  %val = load i32, i32* @global_var
  ret i32 %val
}