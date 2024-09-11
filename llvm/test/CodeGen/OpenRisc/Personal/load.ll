; RUN: llc -march=openrisc < %s | FileCheck %s

define i32 @lwz(i32* %ptr) {
; CHECK: l.lwz r{{[0-9]+}}, 0(r{{[0-9]+}})
  %val = load i32, i32* %ptr
  ret i32 %val
}