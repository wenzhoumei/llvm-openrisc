; RUN: llc -march=openrisc < %s | FileCheck %s

define void @sw(i32 %val, i32* %ptr) {
; CHECK: l.sw 0(r4), r3
  store i32 %val, i32* %ptr
  ret void
}