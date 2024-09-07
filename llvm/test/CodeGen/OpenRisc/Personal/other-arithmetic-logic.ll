; RUN: llc -march=openrisc < %s | FileCheck %s

; Test RI subtraction
define i32 @subi(i32 %a) {
; CHECK: l.addi
; CHECK: -10
  %diff = sub i32 %a, 10
  ret i32 %diff
}

; Test RR multiplication
define i32 @mul(i32 %a, i32 %b) {
; CHECK: l.jal __mulsi3
  %prod = mul i32 %a, %b
  ret i32 %prod
}

; Test RR division
define i32 @div(i32 %a, i32 %b) {
; CHECK: l.jal __divsi3
  %quot = sdiv i32 %a, %b
  ret i32 %quot
}

; Test RI multiplication
define i32 @muli(i32 %a) {
; CHECK: l.jal __mulsi3
  %prod = mul i32 %a, 10
  ret i32 %prod
}


; Test 64-bit add
define i64 @addc(i64 %a, i64 %b) {
; CHECK: l.addc
  %sum = add i64 %a, %b
  ret i64 %sum
}


; Test RI division
define i32 @divi(i32 %a) {
; CHECK: l.jal __divsi3
  %quot = sdiv i32 %a, 10
  ret i32 %quot
}