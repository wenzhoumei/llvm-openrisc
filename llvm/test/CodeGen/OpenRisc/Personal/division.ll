; RUN: llc -march=openrisc < %s | FileCheck %s

; Test RR division
define i32 @div(i32 %a, i32 %b) {
; CHECK: l.jal __divsi3
  %quot = sdiv i32 %a, %b
  ret i32 %quot
}

; Test RI division
define i32 @divi(i32 %a) {
; CHECK: l.jal __divsi3
  %quot = sdiv i32 %a, 10
  ret i32 %quot
}

; Test division by power of 2
define i32 @divi_pow2(i32 %a) {
; CHECK: l.srli r11, r3, 3
  %quot = udiv i32 %a, 8
  ret i32 %quot
}