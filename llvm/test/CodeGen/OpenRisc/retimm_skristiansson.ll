; RUN: llc -march=openrisc < %s | FileCheck %s

define i32 @ret0() {
entry:
  ret i32 0
}

; CHECK: ret0:
; CHECK: l.movhi r11, 0
