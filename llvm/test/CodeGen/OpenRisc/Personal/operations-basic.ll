; RUN: llc -march=openrisc < %s | FileCheck %s
; Checks 32-bit arithmetic and logic operations with 1-1 mappings

define i32 @add(i32 %a, i32 %b) {
; CHECK: l.add
  %sum = add i32 %a, %b
  ret i32 %sum
}

define i32 @sub(i32 %a, i32 %b) {
; CHECK: l.sub
  %diff = sub i32 %a, %b
  ret i32 %diff
}

define i32 @or(i32 %a, i32 %b) {
; CHECK: l.or
  %res = or i32 %a, %b
  ret i32 %res
}

define i32 @and(i32 %a, i32 %b) {
; CHECK: l.and
  %res = and i32 %a, %b
  ret i32 %res
}

define i32 @xor(i32 %a, i32 %b) {
; CHECK: l.xor
  %res = xor i32 %a, %b
  ret i32 %res
}

define i32 @addi(i32 %a) {
; CHECK: l.addi
; CHECK: 10
  %sum = add i32 %a, 10
  ret i32 %sum
}

define i32 @andi(i32 %a) {
; CHECK: l.andi
  %res = and i32 %a, 10
  ret i32 %res
}

define i32 @ori(i32 %a) {
; CHECK: l.ori
  %res = or i32 %a, 10
  ret i32 %res
}

define i32 @xori(i32 %a) {
; CHECK: l.xori
  %res = xor i32 %a, 10
  ret i32 %res
}

define i32 @sll(i32 %a, i32 %b) {
; CHECK: l.sll
  %result = shl i32 %a, %b
  ret i32 %result
}

define i32 @srl(i32 %a, i32 %b) {
; CHECK: l.srl
  %result = lshr i32 %a, %b
  ret i32 %result
}

define i32 @sra(i32 %a, i32 %b) {
; CHECK: l.sra
  %result = ashr i32 %a, %b
  ret i32 %result
}

define i32 @slli(i32 %a) {
; CHECK: l.slli
  %result = shl i32 %a, 5
  ret i32 %result
}

define i32 @srli(i32 %a) {
; CHECK: l.srli
  %result = lshr i32 %a, 5
  ret i32 %result
}

define i32 @srai(i32 %a) {
; CHECK: l.srai
  %result = ashr i32 %a, 5
  ret i32 %result
}