; RUN: llc -march=openrisc < %s | FileCheck %s

; Test division
define i32 @div(i32 %a, i32 %b) {
; CHECK: div
  %quot = sdiv i32 %a, %b
  ret i32 %quot
}