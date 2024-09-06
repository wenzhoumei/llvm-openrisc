; RUN: llc -march=openrisc < %s | FileCheck %s

; TODO: figure out why not removing l.j in optimization

define i32 @branch(i32 %a, i32 %b) {
; CHECK: {{l\.sfne|l\.sfeq}}
; CHECK-NEXT: l.bf
  %cmp = icmp eq i32 %a, %b
  br i1 %cmp, label %equal, label %notequal

equal:                                          
  ret i32 1

notequal:                                       
  ret i32 0
}
