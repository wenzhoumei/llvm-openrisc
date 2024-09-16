;;; Checks 16-bit shift operations
; RUN: llc -march=openrisc < %s | FileCheck %s

;; Check all shift operations
define i16 @sll(i16 %a, i16 %b) {
; CHECK: l.andi r4, r4, 65535
; CHECK-NEXT: l.sll r11, r3, r4
  %res = shl i16 %a, %b
  ret i16 %res
}

define i16 @srl(i16 %a, i16 %b) {
; CHECK: l.andi r4, r4, 65535
; CHECK-NEXT: l.andi r3, r3, 65535
  %res = lshr i16 %a, %b
  ret i16 %res
}

define i16 @sra(i16 %a, i16 %b) {
; CHECK: l.andi r4, r4, 65535
; CHECK-NEXT: l.slli r3, r3, 16
; CHECK-NEXT: l.srai r3, r3, 16
; CHECK-NEXT: l.sra r11, r3, r4
  %res = ashr i16 %a, %b
  ret i16 %res
}

define i16 @slli(i16 %a) {
; CHECK: l.slli r11, r3, 5
  %res = shl i16 %a, 5
  ret i16 %res
}

define i16 @srli(i16 %a) {
; CHECK: l.andi r3, r3, 65504
; CHECK-NEXT: l.srli r11, r3, 5
  %res = lshr i16 %a, 5
  ret i16 %res
}

define i16 @srai(i16 %a) {
; CHECK: l.slli r3, r3, 16
; CHECK-NEXT: l.srai r11, r3, 21
  %res = ashr i16 %a, 5
  ret i16 %res
}