; RUN: llc -march=openrisc < %s | FileCheck %s

define i32 @compare(i32 %a, i32 %b) {
; CHECK: l.sflts
; CHECK: l.bf
  %lt = icmp slt i32 %a, %b
  %res = zext i1 %lt to i32
  ret i32 %res
}