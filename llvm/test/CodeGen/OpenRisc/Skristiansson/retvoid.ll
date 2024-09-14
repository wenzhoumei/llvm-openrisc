; RUN: llc -march=openrisc < %s | FileCheck %s

define void @foo() {
entry:
; CHECK: foo:
; CHECK: l.jr	r9
; CHECK: l.nop
  ret void
}
