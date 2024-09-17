; RUN: llc -march=openrisc < %s | FileCheck %s

; Test function return
define i32 @return_arg(i32 %a) {
; CHECK l.or r11, r3, r3
; CHECK: l.jr 9
  ret i32 %a
}