; RUN: llc -march=openrisc < %s | FileCheck %s

; Test RR multiplication
define i32 @mul(i32 %a, i32 %b) {
; CHECK: l.jal __mulsi3
  %prod = mul i32 %a, %b
  ret i32 %prod
}

; Test RI multiplication
define i32 @muli(i32 %a) {
; CHECK: l.jal __mulsi3
  %prod = mul i32 %a, 10
  ret i32 %prod
}
