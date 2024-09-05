; RUN: llc -march=openrisc < %s | FileCheck %s

define i32 @retarg(i32 %a) nounwind readnone {
entry:
  ret i32 %a
}

; CHECK: retarg:
; CHECK: l.ori	r11, r3, 0
