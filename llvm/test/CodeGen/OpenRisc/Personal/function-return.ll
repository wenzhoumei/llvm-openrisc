; RUN: llc -march=openrisc < %s | FileCheck %s

; Test function return
define i32 @return_arg(i32 %a) {
; CHECK: l.jr lr
  ret i32 %a
}