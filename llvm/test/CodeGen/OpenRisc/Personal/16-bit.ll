;;; Checks basic 16-bit operations
; RUN: llc -march=openrisc < %s | FileCheck %s

define i16 @add(i16 %a, i16 %b) {
; CHECK: l.add r11, r3, r4
  %sum = add i16 %a, %b
  ret i16 %sum
}

define i16 @sub(i16 %a, i16 %b) {
; CHECK: l.sub r11, r3, r4
  %diff = sub i16 %a, %b
  ret i16 %diff
}

define i16 @or(i16 %a, i16 %b) {
; CHECK: l.or r11, r3, r4
  %res = or i16 %a, %b
  ret i16 %res
}

define i16 @and(i16 %a, i16 %b) {
; CHECK: l.and r11, r3, r4
  %res = and i16 %a, %b
  ret i16 %res
}

define i16 @xor(i16 %a, i16 %b) {
; CHECK: l.xor r11, r3, r4
  %res = xor i16 %a, %b
  ret i16 %res
}

define i16 @addi(i16 %a) {
; CHECK: l.addi r11, r3, 10
  %sum = add i16 %a, 10
  ret i16 %sum
}

define i16 @andi(i16 %a) {
; CHECK: l.andi r11, r3, 10
  %res = and i16 %a, 10
  ret i16 %res
}

define i16 @ori(i16 %a) {
; CHECK: l.ori r11, r3, 10
  %res = or i16 %a, 10
  ret i16 %res
}

define i16 @xori(i16 %a) {
; CHECK: l.xori r11, r3, 10
  %res = xor i16 %a, 10
  ret i16 %res
}