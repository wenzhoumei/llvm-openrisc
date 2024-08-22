; RUN: llc -march=openrisc < %s | FileCheck%s

define i32 @caller(i32 %x) {
; CHECK: call
  %result = call i32 @callee(i32 %x)
  ret i32 %result
}

define i32 @callee(i32 %y) {
; CHECK: ret
  ret i32 %y
}
