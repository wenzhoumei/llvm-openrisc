; RUN: llc -march=openrisc < %s | FileCheck %s

; Test addition
define i32 @add(i32 %a, i32 %b) {
; CHECK: add
  %sum = add i32 %a, %b
  ret i32 %sum
}

; Test subtraction
define i32 @sub(i32 %a, i32 %b) {
; CHECK: sub
  %diff = sub i32 %a, %b
  ret i32 %diff
}