; RUN: llc -march=openrisc < %s | FileCheck %s

; Test multiplication
define i32 @mul(i32 %a, i32 %b) {
; CHECK: mul
  %prod = mul i32 %a, %b
  ret i32 %prod
}
