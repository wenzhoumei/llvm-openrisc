; Test 64-bit add
define i64 @add(i64 %a, i64 %b) {
; CHECK: l.add
  %sum = add i64 %a, %b
  ret i64 %sum
}