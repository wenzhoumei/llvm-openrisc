; RUN: llc -march=openrisc < %s | FileCheck %s

define i32 @lwz(i32* %ptr) {
; CHECK: l.lwz r11, 0(r3)
  %val = load i32, i32* %ptr
  ret i32 %val
}