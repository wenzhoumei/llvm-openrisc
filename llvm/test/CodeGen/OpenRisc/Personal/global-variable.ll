; RUN: llc -march=openrisc < %s | FileCheck %s

@global_var = global i32 3

define i32 @return_global() {
; CHECK: l.movhi r3, hi(global_var)
; CHECK: l.ori r3, r3, lo(global_var)
  %val = load i32, i32* @global_var
  ret i32 @global_var
}