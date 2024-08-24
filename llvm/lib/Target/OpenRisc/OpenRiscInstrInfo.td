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
// Includes: l.add, l.addc, l.and, l.div, l.divu, l.or, l.sub, l.xor
//===----------------------------------------------------------------------===//

class F1<bits<6> opcode1, bits<2> opcode2, bits<4> opcode3, string instrAsm, SDPatternOperator opNode, bit isComm = 0>
  : F1_Inst<opcode1, opcode2, opcode3, (outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
            instrAsm#"\t$rd, $ra, $rb", 
            [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))]> {
  let isCommutable = isComm;
}

def ADD  : F1<0x38, 0x0, 0x0, "l.add", add, 1>;
def ADDC : F1<0x38, 0x0, 0x1, "l.addc", addc>;
def AND  : F1<0x38, 0x0, 0x4, "l.and", and, 1>;
def DIV  : F1<0x38, 0x0, 0xC, "l.div", sdiv>;
def DIVU : F1<0x38, 0x0, 0xD, "l.divu", udiv>;
def OR   : F1<0x38, 0x0, 0x6, "l.or", or, 1>;
def SUB  : F1<0x38, 0x0, 0x2, "l.sub", sub>;
def XOR  : F1<0x38, 0x0, 0x8, "l.xor", xor, 1>;

//===----------------------------------------------------------------------===//
// Format 2 Instructions
// Includes: l.addi, l.addic, l.andi, l.lbs, l.lbz, l.ld, l.lhs, l.lhz, l.lws, l.lwz, l.mfspr, l.mtspr, l.ori, l.xori
//===----------------------------------------------------------------------===//

class F2<bits<6> opcode, string instrAsm, SDPatternOperator opNode>
  : F2_Inst<opcode, (outs GPR:$rd), (ins GPR:$ra, simm16:$imm),
            instrAsm#"\t$rd, $ra, $imm", 
            [(set GPR:$rd, (opNode GPR:$ra, simm16:$imm))]>;

def ADDI  : F2<0x04, "l.addi", add>;
def ANDI  : F2<0x05, "l.andi", and>;
def ORI   : F2<0x06, "l.ori", or>;
def XORI  : F2<0x07, "l.xori", xor>;
def LBS   : F2<0x21, "l.lbs", load>;
def LBZ   : F2<0x23, "l.lbz", load>;
def LHS   : F2<0x25, "l.lhs", load>;
def LHZ   : F2<0x27, "l.lhz", load>;
def LWS   : F2<0x2B, "l.lws", load>;
def LWZ   : F2<0x2D, "l.lwz", load>;
def MFSR  : F2<0x35, "l.mfspr", read_register>;
def MTSR  : F2<0x36, "l.mtspr", write_register>;

//===----------------------------------------------------------------------===//
// Format 3 Instructions
// Includes: l.sll, l.sra, l.srl
//===----------------------------------------------------------------------===//

class F3<bits<4> opcode2, bits<4> opcode3, string instrAsm, SDPatternOperator opNode>
  : F3_Inst<0x0F, opcode2, opcode3, (outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
            instrAsm#"\t$rd, $ra, $rb", 
            [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))]>;

def SLL  : F3<0x0, 0x1, "l.sll", shl>;
def SRA  : F3<0x0, 0x3, "l.sra", sra>;
def SRL  : F3<0x0, 0x2, "l.srl", srl>;

//===----------------------------------------------------------------------===//
// Format 4 Instructions
// Includes: l.slli, l.srai, l.srli
//===----------------------------------------------------------------------===//

class F4<bits<2> opcode2, string instrAsm, SDPatternOperator opNode>
  : F4_Inst<0x04, opcode2, (outs GPR:$rd), (ins GPR:$ra, uimm6:$imm),
            instrAsm#"\t$rd, $ra, $imm", 
            [(set GPR:$rd, (opNode GPR:$ra, uimm6:$imm))]>;

def SLLI  : F4<0x0, "l.slli", shl>;
def SRAI  : F4<0x1, "l.srai", sra>;
def SRLI  : F4<0x2, "l.srli", srl>;

//===----------------------------------------------------------------------===//
// Format 5 Instructions
// Includes: l.bf, l.bnf, l.j, l.jal
//===----------------------------------------------------------------------===//

class F5<bits<6> opcode, string instrAsm, SDPatternOperator branchOp>
  : F5_Inst<opcode, (outs), (ins brtarget:$target),
            instrAsm#"\t$target", 
            [(branchOp brtarget:$target)]>;

def J   : F5<0x09, "l.j", br>;
def JAL : F5<0x0A, "l.jal", br>;
def BF  : F5<0x01, "l.bf", brcond>;
def BNF : F5<0x02, "l.bnf", brcond_not>;

//===----------------------------------------------------------------------===//
// Format 6 Instructions
// Includes: l.jalr, l.jr
//===----------------------------------------------------------------------===//

class F6<bits<6> opcode, string instrAsm, SDPatternOperator branchOp>
  : F6_Inst<opcode, (outs), (ins GPR:$rb),
            instrAsm#"\t$rb", 
            [(branchOp GPR:$rb)]>;

def JR   : F6<0x10, "l.jr", jr>;
def JALR : F6<0x12, "l.jalr", jalr>;

//===----------------------------------------------------------------------===//
// Format 7 Instructions
// Includes: l.sb, l.sd, l.sh, l.sw
//===----------------------------------------------------------------------===//

class F7<bits<6> opcode, string instrAsm, SDPatternOperator opNode>
  : F7_Inst<opcode, (outs), (ins GPR:$ra, GPR:$rb, simm11:$imm),
            instrAsm#"\t$ra, $rb, $imm", 
            [(opNode GPR:$ra, GPR:$rb, simm11:$imm)]>;

def SB : F7<0x28, "l.sb", store>;
def SD : F7<0x2A, "l.sd", store>;
def SH : F7<0x2B, "l.sh", store>;
def SW : F7<0x2D, "l.sw", store>;

//===----------------------------------------------------------------------===//
// Format 8 Instructions
// Includes: l.sfeq, l.sfges, l.sfgeu, l.sfgts, l.sfgtu, l.sfles, l.sfleu, l.sflts, l.sfltu, l.sfne
//===----------------------------------------------------------------------===//

class F8<bits<11> opcode, string instrAsm, CondCode CC>
  : F8_Inst<opcode, (outs), (ins GPR:$ra, GPR:$rb),
            instrAsm#"\t$ra, $rb", 
            [(setcc CC, GPR:$ra, GPR:$rb)]>;

def SFEQ  : F8<0x280, "l.sfeq", SETEQ>;
def SFGES : F8<0x290, "l.sfges", SETGES>;
def SFGEU : F8<0x2A0, "l.sfgeu", SETGEU>;
def SFGTS : F8<0x2B0, "l.sfgts", SETGTS>;
def SFLES : F8<0x2C0, "l.sfles", SETLES>;
def SFLEU : F8<0x2D0, "l.sfleu", SETLEU>;
def SFLTS : F8<0x2E0, "l.sflts", SETLTS>;
def SFLTU : F8<0x2F0, "l.sfltu", SETLTU>;
def SFNE  : F8<0x300, "l.sfne", SETNE>;

//===----------------------------------------------------------------------===//
// Format 9 Instructions
// Includes: l.movhi
//===----------------------------------------------------------------------===//

class F9<bits<6> opcode1, bits<1> opcode2, string instrAsm, SDPatternOperator opNode>
  : F9_Inst<opcode1, opcode2, (outs GPR:$rd), (ins simm16:$imm),
            instrAsm#"\t$rd, $imm", 
            [(set GPR:$rd, (opNode simm16:$imm))]>;

def MOVHI : F9<0x0A, 0x1, "l.movhi", movhi>;

//===----------------------------------------------------------------------===//
// Format 10 Instructions
// Includes: l.nop
//===----------------------------------------------------------------------===//

class F10<bits<8> opcode, string instrAsm, list<dag> pattern>
  : F10_Inst<opcode, (outs), (ins), instrAsm, pattern>;

def NOP : F10<0x15, "l.nop", []>;

//===----------------------------------------------------------------------===//
// Format 11 Instructions
// Includes: l.rfe
//===----------------------------------------------------------------------===//

class F11<bits<6> opcode, string instrAsm, list<dag> pattern>
  : F11_Inst<opcode, (outs), (ins), instrAsm, pattern>;

def RFE : F11<0x48, "l.rfe", []>;

//===----------------------------------------------------------------------===//
// Format 12 Instructions
// Includes: l.sys
//===----------------------------------------------------------------------===//

class F12<bits<16> opcode, string instrAsm, list<dag> pattern>
  : F12_Inst<opcode, (outs), (ins simm16:$imm), instrAsm#"\t$imm", pattern>;

def SYS : F12<0x0000, "l.sys", []>;