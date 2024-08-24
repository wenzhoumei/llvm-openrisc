//===- OpenRiscRegisterInfo.td - OpenRisc Register definitions -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===---------------------------------------------------------------------------===//

// Base class for all OpenRisc registers
class OpenRisc_Reg<string n> : Register<n> {
  let Namespace = "OpenRisc";
}

//===----------------------------------------------------------------------===//
// General-purpose registers
//===----------------------------------------------------------------------===//

// Base class for special purpose registers identified by 5-bit group and 10-bit register number

// OpenRisc general-purpose registers identified by 5 bit 
class GPR<bits<5> num, string n, list<string> alt = []> : OpenRisc_Reg<n> {
  let HWEncoding{4-0} = num;
  let AltNames = alt;
}

// Fixed zero register
def R0 : GPR<0, "r0", ["zero"]>;

// Stack pointer
def R1 : GPR<1, "r1", ["sp"]>, DwarfRegNum<[1]>;

// Frame pointer (optional)
def R2 : GPR<2, "r2", ["fp"]>, DwarfRegNum<[2]>;

// Function parameter registers
def R3 : GPR<3, "r3">, DwarfRegNum<[3]>;
def R4 : GPR<4, "r4">, DwarfRegNum<[4]>;
def R5 : GPR<5, "r5">, DwarfRegNum<[5]>;
def R6 : GPR<6, "r6">, DwarfRegNum<[6]>;
def R7 : GPR<7, "r7">, DwarfRegNum<[7]>;
def R8 : GPR<8, "r8">, DwarfRegNum<[8]>;

// Link address register
def R9 : GPR<9, "r9", ["lr"]>, DwarfRegNum<[9]>;

// Thread local storage
def R10 : GPR<10, "r10">, DwarfRegNum<[10]>;

// Return value register
def R11 : GPR<11, "r11", ["rv"]>, DwarfRegNum<[11]>;
def R12 : GPR<12, "r12", ["rvh"]>, DwarfRegNum<[12]>; 


// Callee-saved registers
def R13 : GPR<13, "r13">, DwarfRegNum<[13]>;
def R14 : GPR<14, "r14">, DwarfRegNum<[14]>;
def R16 : GPR<16, "r16">, DwarfRegNum<[16]>;
def R18 : GPR<18, "r18">, DwarfRegNum<[18]>;
def R20 : GPR<20, "r20">, DwarfRegNum<[20]>;
def R22 : GPR<22, "r22">, DwarfRegNum<[22]>;
def R24 : GPR<24, "r24">, DwarfRegNum<[24]>;
def R26 : GPR<26, "r26">, DwarfRegNum<[26]>;
def R28 : GPR<28, "r28">, DwarfRegNum<[28]>;
def R30 : GPR<30, "r30">, DwarfRegNum<[30]>;

// Temporary registers
def R15 : GPR<15, "r15">, DwarfRegNum<[15]>;
def R17 : GPR<17, "r17">, DwarfRegNum<[17]>;
def R19 : GPR<19, "r19">, DwarfRegNum<[19]>;
def R21 : GPR<21, "r21">, DwarfRegNum<[21]>;
def R23 : GPR<23, "r23">, DwarfRegNum<[23]>;
def R25 : GPR<25, "r25">, DwarfRegNum<[25]>;
def R27 : GPR<27, "r27">, DwarfRegNum<[27]>;
def R29 : GPR<29, "r29">, DwarfRegNum<[29]>;
def R31 : GPR<31, "r31">, DwarfRegNum<[31]>;



//===----------------------------------------------------------------------===//
// Special-purpose registers
//===----------------------------------------------------------------------===//

// Base class for special purpose registers identified by 5-bit group and 10-bit register number
class SPR<bits<5> group, bits<11> num, string n> : OpenRisc_Reg<n> {
  let HWEncoding{10-0} = num;
  let HWEncoding{15-11} = group;
}

// Group 0: System Control and Status registers
def VR : SPR<0, 0, "VR">;     // Version register
def UPR : SPR<0, 1, "UPR">;   // Unit Present register
def CPUCFGR : SPR<0, 2, "CPUCFGR">;  // CPU Configuration register
def DMMUCFGR : SPR<0, 3, "DMMUCFGR">;  // Data MMU Configuration register
def IMMUCFGR : SPR<0, 4, "IMMUCFGR">;  // Instruction MMU Configuration register
def DCCFGR : SPR<0, 5, "DCCFGR">;  // Data Cache Configuration register
def ICCFGR : SPR<0, 6, "ICCFGR">;  // Instruction Cache Configuration register
def DCFGR : SPR<0, 7, "DCFGR">;  // Debug Configuration register
def PCCFGR : SPR<0, 8, "PCCFGR">;  // Performance Counters Configuration register
def VR2 : SPR<0, 9, "VR2">;   // Version register 2
def AVR : SPR<0, 10, "AVR">;  // Architecture version register
def EVBAR : SPR<0, 11, "EVBAR">;  // Exception vector base address register
def AECR : SPR<0, 12, "AECR">;  // Arithmetic Exception Control Register
def AESR : SPR<0, 13, "AESR">;  // Arithmetic Exception Status Register
def NPC : SPR<0, 16, "NPC">;  // Next PC register
def SR : SPR<0, 17, "SR">;    // Supervision register
def PPC : SPR<0, 18, "PPC">;  // Previous PC register
def FPCSR : SPR<0, 20, "FPCSR">;  // FP Control Status register

// Exception PC, EA, and SR registers (0-15)
foreach i = 0-15 in {
  def EPCR#i : SPR<0, !add(32,i), "EPCR"#i>;
  def EEAR#i : SPR<0, !add(48,i), "EEAR"#i>;
  def ESR#i : SPR<0, !add(64,i), "ESR"#i>;
}

// Implementation-specific registers (ISR0-ISR7)
foreach i = 21-28 in {
  def ISR#i : SPR<0, i, "ISR"#i>;
}

// Core and Core Count registers
def COREID : SPR<0, 128, "COREID">;   // Core Identifier Register
def NUMCORES : SPR<0, 129, "NUMCORES">;   // Number of Cores Register

// GPRs mapped to SPR space
foreach i = 1024-1535 in {
  def GPR_SPR#i : SPR<0, i, "GPR_SPR"#i>;
}

//===----------------------------------------------------------------------===//
// Register classes
//===----------------------------------------------------------------------===//

// General purpose register class in allocation order that prioritizes non-callee-saved registers
def GPR_Class : RegisterClass<"OpenRisc", [i32], 32, (add
  R3, R4, R5, R6, R7, R8, R9, R10, R11,
  R12, R13, R14, R16, R18, R20, R22, R24, 
  R26, R28, R30, R15, R17, R19, R21, R23, 
  R25, R27, R29, R31, R1, R2, R0)>;

// Register class for EPCR registers
def EPCR_Class : RegisterClass<"OpenRisc", [i32], 32, 
  (add (sequence "EPCR%u", 0, 15))>;

// Register class for EEAR registers
def EEAR_Class : RegisterClass<"OpenRisc", [i32], 32, 
  (add (sequence "EEAR%u", 0, 15))>;

// Register class for ESR registers
def ESR_Class : RegisterClass<"OpenRisc", [i32], 32, 
  (add (sequence "ESR%u", 0, 15))>;

// Register class for ISR registers
def ISR_Class : RegisterClass<"OpenRisc", [i32], 32, 
  (add (sequence "ISR%u", 0, 7))>;

// Register class for GPRs mapped to SPR space
def GPR_SPRClass : RegisterClass<"OpenRisc", [i32], 32, 
  (add (sequence "GPR_SPR%u", 0, 511))>;