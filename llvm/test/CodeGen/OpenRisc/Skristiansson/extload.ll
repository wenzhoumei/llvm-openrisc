; RUN: llc -march=openrisc < %s | FileCheck %s

define i32 @extload(i16 zeroext %a) {
entry:
  %a.addr = alloca i16, align 2
  %c = alloca i8, align 1
  store i16 %a, i16* %a.addr, align 2    ; Store the parameter %a into %a.addr
  %0 = load i16, i16* %a.addr, align 2   ; Corrected load instruction
  %conv = trunc i16 %0 to i8
  store i8 %conv, i8* %c, align 1
  ret i32 0
}
