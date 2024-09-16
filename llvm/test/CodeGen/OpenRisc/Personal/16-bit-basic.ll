; RUN: llc -march=openrisc < %s | FileCheck %s
; Checks 16-bit arithmetic and logic operations with 1-1 mappings

define i16 @add(i16 %a, i16 %b) {
; CHECK: l.add
  %sum = add i16 %a, %b
  ret i16 %sum
}

define i16 @sub(i16 %a, i16 %b) {
; CHECK: l.sub
  %diff = sub i16 %a, %b
  ret i16 %diff
}

define i16 @or(i16 %a, i16 %b) {
; CHECK: l.or
  %res = or i16 %a, %b
  ret i16 %res
}

define i16 @and(i16 %a, i16 %b) {
; CHECK: l.and
  %res = and i16 %a, %b
  ret i16 %res
}

define i16 @xor(i16 %a, i16 %b) {
; CHECK: l.xor
  %res = xor i16 %a, %b
  ret i16 %res
}

define i16 @addi(i16 %a) {
; CHECK: l.addi
; CHECK: 10
  %sum = add i16 %a, 10
  ret i16 %sum
}

define i16 @andi(i16 %a) {
; CHECK: l.andi
  %res = and i16 %a, 10
  ret i16 %res
}

define i16 @ori(i16 %a) {
; CHECK: l.ori
  %res = or i16 %a, 10
  ret i16 %res
}

define i16 @xori(i16 %a) {
; CHECK: l.xori
  %res = xor i16 %a, 10
  ret i16 %res
}

define i16 @sll(i16 %a, i16 %b) {
; CHECK: l.sll
  %result = shl i16 %a, %b
  ret i16 %result
}

define i16 @srl(i16 %a, i16 %b) {
; CHECK: l.srl
  %result = lshr i16 %a, %b
  ret i16 %result
}

define i16 @sra(i16 %a, i16 %b) {
; CHECK: l.sra
  %result = ashr i16 %a, %b
  ret i16 %result
}

define i16 @slli(i16 %a) {
; CHECK: l.slli
  %result = shl i16 %a, 5
  ret i16 %result
}

define i16 @srli(i16 %a) {
; CHECK: l.srli
  %result = lshr i16 %a, 5
  ret i16 %result
}

define i16 @srai(i16 %a) {
; CHECK: l.srai
  %result = ashr i16 %a, 5
  ret i16 %result
}