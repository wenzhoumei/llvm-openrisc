; RUN: llc -march=openrisc < %s | FileCheck%s

define i32 @branch(i32 %a, i32 %b) {
; CHECK: beq
; CHECK: bne
  %cmp = icmp eq i32 %a, %b
  br i1 %cmp, label %equal, label %notequal

equal:                                          
  ret i32 1

notequal:                                       
  ret i32 0
}
