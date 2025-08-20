//===- OR1KRegisterInfo.td - OR1K Register definitions -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

class OR1KReg<string n> : Register<n> {
  let Namespace = "OR1K";
}

//===----------------------------------------------------------------------===//
// General-purpose registers
//===----------------------------------------------------------------------===//

class GPRReg<bits<5> num, string n, list<string> alt = []> : OR1KReg<n> {
  let HWEncoding{4-0} = num;
  let AltNames = alt;
}

let isConstant = true in {
// Fixed zero register
def R0 : GPRReg<0, "r0", ["zero"]>;
}

// Stack pointer
def R1 : GPRReg<1, "r1", ["sp"]>, DwarfRegNum<[1]>;

// Frame pointer (optional)
def R2 : GPRReg<2, "r2", ["fp"]>, DwarfRegNum<[2]>;

// Function parameter registers
def R3 : GPRReg<3, "r3">, DwarfRegNum<[3]>;
def R4 : GPRReg<4, "r4">, DwarfRegNum<[4]>;
def R5 : GPRReg<5, "r5">, DwarfRegNum<[5]>;
def R6 : GPRReg<6, "r6">, DwarfRegNum<[6]>;
def R7 : GPRReg<7, "r7">, DwarfRegNum<[7]>;
def R8 : GPRReg<8, "r8">, DwarfRegNum<[8]>;

// Link address register
def R9 : GPRReg<9, "r9", ["lr"]>, DwarfRegNum<[9]>;

// Thread local storage
def R10 : GPRReg<10, "r10">, DwarfRegNum<[10]>;

// Return value register
def R11 : GPRReg<11, "r11", ["rv"]>, DwarfRegNum<[11]>;
def R12 : GPRReg<12, "r12", ["rvh"]>, DwarfRegNum<[12]>;

// Callee-saved registers
def R13 : GPRReg<13, "r13">, DwarfRegNum<[13]>;
def R14 : GPRReg<14, "r14">, DwarfRegNum<[14]>;
def R16 : GPRReg<16, "r16">, DwarfRegNum<[16]>;
def R18 : GPRReg<18, "r18">, DwarfRegNum<[18]>;
def R20 : GPRReg<20, "r20">, DwarfRegNum<[20]>;
def R22 : GPRReg<22, "r22">, DwarfRegNum<[22]>;
def R24 : GPRReg<24, "r24">, DwarfRegNum<[24]>;
def R26 : GPRReg<26, "r26">, DwarfRegNum<[26]>;
def R28 : GPRReg<28, "r28">, DwarfRegNum<[28]>;
def R30 : GPRReg<30, "r30">, DwarfRegNum<[30]>;

// Temporary registers
def R15 : GPRReg<15, "r15">, DwarfRegNum<[15]>;
def R17 : GPRReg<17, "r17">, DwarfRegNum<[17]>;
def R19 : GPRReg<19, "r19">, DwarfRegNum<[19]>;
def R21 : GPRReg<21, "r21">, DwarfRegNum<[21]>;
def R23 : GPRReg<23, "r23">, DwarfRegNum<[23]>;
def R25 : GPRReg<25, "r25">, DwarfRegNum<[25]>;
def R27 : GPRReg<27, "r27">, DwarfRegNum<[27]>;
def R29 : GPRReg<29, "r29">, DwarfRegNum<[29]>;
def R31 : GPRReg<31, "r31">, DwarfRegNum<[31]>;

//===----------------------------------------------------------------------===//
// Flags
//===----------------------------------------------------------------------===//

// Flags are modelled as registers to not create false dependencies.
def SR_FFlag : OR1KReg<"FFlag">;

//===----------------------------------------------------------------------===//
// Register classes
//===----------------------------------------------------------------------===//

def GPR : RegisterClass<"OR1K", [i32], 32, (add
  R3, R4, R5, R6, R7, R8, R9, R10, R11,
  R12, R13, R14, R16, R18, R20, R22, R24,
  R26, R28, R30, R15, R17, R19, R21, R23,
  R25, R27, R29, R31, R1, R2, R0)>;

def SR_F : RegisterClass<"OR1K", [i1], 32, (add SR_FFlag)>;
