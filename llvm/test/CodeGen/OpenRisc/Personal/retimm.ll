; RUN: llc -march=openrisc < %s | FileCheck %s

define i32 @ret0() {
; CHECK: l.movhi r11, hi(0)
  ret i32 0
}

define i32 @retn65536() {
; CHECK: l.movhi r11, hi(-65536)
  ret i32 -65536
}

define i32 @ret1() {
; CHECK: l.ori r11, r0, 1
  ret i32 1
}

define i32 @ret12345678() {
; CHECK: l.movhi
; CHECK: hi(12320768)
; CHECK: l.ori
; CHECK: 24910
  ret i32 12345678
}

define i32 @retn12345678() {
; CHECK: l.movhi
; CHECK: hi(-12386304)
; CHECK: l.ori
; CHECK: 40626
  ret i32 -12345678
}