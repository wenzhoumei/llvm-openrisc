; RUN: llc -march=openrisc < %s | FileCheck %s
 
; Test RI subtraction
define i32 @subi(i32 %a) {
; CHECK: l.addi r11, r3, -10
  %diff = sub i32 %a, 10
  ret i32 %diff
}
