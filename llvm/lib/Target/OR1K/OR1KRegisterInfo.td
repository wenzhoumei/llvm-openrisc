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
// Special-purpose registers
//===----------------------------------------------------------------------===//

class SPR<bits<5> group, bits<11> num, string n> : OR1KReg<n> {
  let HWEncoding{10-0} = num;
  let HWEncoding{15-11} = group;
}

def VRReg: SPR<0, 0, "VR">;     // Version register
def UPRReg: SPR<0, 1, "UPR">;   // Unit Present register
def CPUCFGRReg: SPR<0, 2, "CPUCFGR">;  // CPU Configuration register
def DMMUCFGRReg: SPR<0, 3, "DMMUCFGR">;  // Data MMU Configuration register
def IMMUCFGRReg: SPR<0, 4, "IMMUCFGR">;  // Instruction MMU Configuration register
def DCCFGRReg: SPR<0, 5, "DCCFGR">;  // Data Cache Configuration register
def ICCFGRReg: SPR<0, 6, "ICCFGR">;  // Instruction Cache Configuration register
def DCFGRReg: SPR<0, 7, "DCFGR">;  // Debug Configuration register
def PCCFGRReg: SPR<0, 8, "PCCFGR">;  // Performance Counters Configuration register
def VR2Reg : SPR<0, 9, "VR2">;   // Version register 2
def AVRReg: SPR<0, 10, "AVR">;  // Architecture version register
def EVBARReg: SPR<0, 11, "EVBAR">;  // Exception vector base address register
def AECRReg: SPR<0, 12, "AECR">;  // Arithmetic Exception Control Register
def AESRReg: SPR<0, 13, "AESR">;  // Arithmetic Exception Status Register
def NPCReg : SPR<0, 16, "NPC">;  // Next PC register
def SRReg : SPR<0, 17, "SR">;    // Supervision register
def PPCReg : SPR<0, 18, "PPC">;  // Previous PC register
def FPCSRReg: SPR<0, 20, "FPCSR">;  // FP Control Status register

foreach i = 0-15 in {
  def EPCRReg#i : SPR<0, !add(32,i), "EPCR"#i>;
  def EEARReg#i : SPR<0, !add(48,i), "EEAR"#i>;
  def ESRReg#i : SPR<0, !add(64,i), "ESR"#i>;
}

foreach i = 0-7 in {
  def ISRReg#i : SPR<0, !add(21,i), "ISR"#i>;
}

def COREIDReg : SPR<0, 128, "COREID">;   // Core Identifier Register
def NUMCORESReg : SPR<0, 129, "NUMCORES">;   // Number of Cores Register

foreach i = 0-511 in {
  def GPR_SPRReg#i : SPR<0, !add(1024,i), "GPR_SPR"#i>;
}

//===----------------------------------------------------------------------===//
// Pseudo registers (for use in codegen only)
//===----------------------------------------------------------------------===//

// Flag for supervision register
def FFlagPReg : OR1KReg<"FFlag">;

//===----------------------------------------------------------------------===//
// Register classes
//===----------------------------------------------------------------------===//

def GPR : RegisterClass<"OR1K", [i32], 32, (add
  R3, R4, R5, R6, R7, R8, R9, R10, R11,
  R12, R13, R14, R16, R18, R20, R22, R24,
  R26, R28, R30, R15, R17, R19, R21, R23,
  R25, R27, R29, R31, R1, R2, R0)>;

def FFlag : RegisterClass<"OR1K", [i1], 32, (add FFlagPReg)>;

def EPCR : RegisterClass<"OR1K", [i32], 32,
  (add (sequence "EPCRReg%u", 0, 15))>;

def EEAR : RegisterClass<"OR1K", [i32], 32,
  (add (sequence "EEARReg%u", 0, 15))>;

def ESR : RegisterClass<"OR1K", [i32], 32,
  (add (sequence "ESRReg%u", 0, 15))>;

def ISR : RegisterClass<"OR1K", [i32], 32,
  (add (sequence "ISRReg%u", 0, 7))>;

def GPR_SPR : RegisterClass<"OR1K", [i32], 32,
  (add (sequence "GPR_SPRReg%u", 0, 511))>;
