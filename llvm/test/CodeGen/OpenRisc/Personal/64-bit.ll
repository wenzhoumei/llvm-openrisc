; Test 64-bit add
define i64 @addc(i64 %a, i64 %b) {
; CHECK: l.addc
  %sum = add i64 %a, %b
  ret i64 %sum
}