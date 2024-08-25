//===- OpenRiscInstrInfo.td - OpenRISC Instruction Descriptions -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the OpenRISC instructions in TableGen format.
//
//===----------------------------------------------------------------------===//

include "OpenRiscInstrFormats.td"
include "OpenRiscOperands.td"
include "OpenRiscOperators.td"

//===----------------------------------------------------------------------===//
// Format 1 Instructions
//===----------------------------------------------------------------------===//
def ADD  : F1_Inst<0x38, 0x0, 0x0, "l.add", add>;
def ADDC : F1_Inst<0x38, 0x0, 0x1, "l.addc", addc>;
def SUB  : F1_Inst<0x38, 0x0, 0x2, "l.sub", sub>;
def AND  : F1_Inst<0x38, 0x0, 0x3, "l.and", and>;
def OR   : F1_Inst<0x38, 0x0, 0x4, "l.or", or>;
def XOR  : F1_Inst<0x38, 0x0, 0x5, "l.xor", xor>;

//===----------------------------------------------------------------------===//
// Format 2 Instructions
//===----------------------------------------------------------------------===//
def LWZ   : F2_Inst<0x21, "l.lwz", load>;
def LWS   : F2_Inst<0x22, "l.lws", load>;
def LBZ   : F2_Inst<0x23, "l.lbz", load>;
def LBS   : F2_Inst<0x24, "l.lbs", load>;
def LHZ   : F2_Inst<0x25, "l.lhz", load>;
def LHS   : F2_Inst<0x26, "l.lhs", load>;
def ADDI  : F2_Inst<0x27, "l.addi", add>;
def ADDIC : F2_Inst<0x28, "l.addic", addc>;
def ANDI  : F2_Inst<0x29, "l.andi", and>;
def ORI   : F2_Inst<0x2a, "l.ori", or>;
def XORI  : F2_Inst<0x2b, "l.xori", xor>;
def MFSPR : F2_Inst<0x2d, "l.mfspr", read_register>;
def MTSPR : F2_Inst<0x30, "l.mtspr", write_register>;

//===----------------------------------------------------------------------===//
// Format 3 Instructions
//===----------------------------------------------------------------------===//
def SLL  : F3_Inst<0x38, 0x0, 0x8, "l.sll", shl>;
def SRL  : F3_Inst<0x38, 0x1, 0x8, "l.srl", srl>;
def SRA  : F3_Inst<0x38, 0x2, 0x8, "l.sra", sra>;

//===----------------------------------------------------------------------===//
// Format 4 Instructions
//===----------------------------------------------------------------------===//
def SLLI  : F4_Inst<0x2e, 0x0, "l.slli", shl>;
def SRLI  : F4_Inst<0x2e, 0x1, "l.srli", srl>;
def SRAI  : F4_Inst<0x2e, 0x2, "l.srai", sra>;

//===----------------------------------------------------------------------===//
// Format 5 Instructions
//===----------------------------------------------------------------------===//
def J   : F5_Inst<0x0, "l.j", br>;
def JAL : F5_Inst<0x1, "l.jal", br>;
def BNF : F5_Inst<0x3, "l.bnf", brcond_not>;
def BF  : F5_Inst<0x4, "l.bf", brcond>;

//===----------------------------------------------------------------------===//
// Format 6 Instructions
//===----------------------------------------------------------------------===//
def JR   : F6_Inst<0x11, "l.jr", jr>;
def JALR : F6_Inst<0x12, "l.jalr", jalr>;

//===----------------------------------------------------------------------===//
// Format 7 Instructions
//===----------------------------------------------------------------------===//
def SD : F7_Inst<0x34, "l.sd", store>;
def SW : F7_Inst<0x35, "l.sw", store>;
def SB : F7_Inst<0x36, "l.sb", store>;
def SH : F7_Inst<0x37, "l.sh", store>;

//===----------------------------------------------------------------------===//
// Format 8 Instructions
//===----------------------------------------------------------------------===//
def SFEQ  : F8_Inst<0x720, "l.sfeq", SETEQ>;
def SFNE  : F8_Inst<0x721, "l.sfne", SETNE>;
def SFGTU : F8_Inst<0x722, "l.sfgtu", SETGTU>;
def SFGEU : F8_Inst<0x723, "l.sfgeu", SETGEU>;
def SFLTU : F8_Inst<0x724, "l.sfltu", SETLTU>;
def SFLEU : F8_Inst<0x725, "l.sfleu", SETLEU>;

def SFGTS : F8_Inst<0x72a, "l.sfgts", SETGTS>;
def SFGES : F8_Inst<0x72b, "l.sfges", SETGES>;
def SFLTS : F8_Inst<0x72c, "l.sflts", SETLTS>;
def SFLES : F8_Inst<0x72d, "l.sfles", SETLES>;

//===----------------------------------------------------------------------===//
// Format 9 Instructions
//===----------------------------------------------------------------------===//
def MOVHI : F9_Inst<0x6, 0x0, "l.movhi", movhi>;

//===----------------------------------------------------------------------===//
// Format 10 Instructions
//===----------------------------------------------------------------------===//
def NOP : F10_Inst<0x15, "l.nop", []>;

//===----------------------------------------------------------------------===//
// Format 11 Instructions
//===----------------------------------------------------------------------===//
def RFE : F11_Inst<0x9, "l.rfe", []>;

//===----------------------------------------------------------------------===//
// Format 12 Instructions
//===----------------------------------------------------------------------===//
def SYS : F12_Inst<0x2000, "l.sys", []>;