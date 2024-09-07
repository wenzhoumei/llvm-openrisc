; RUN: llc -march=openrisc < %s | FileCheck %s

@global_var = global i32 0

define void @load_store(i32* %a) {
; CHECK: lwz
; CHECK: stw
  %addr = alloca i32
  store i32 42, i32* %addr
  %val = load i32, i32* %a
  ;store i32 %val, i32* @global_var
  ret void
}