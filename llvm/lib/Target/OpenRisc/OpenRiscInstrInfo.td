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
// Format 1 - Arithmetic + logic instructions without immediate
//===----------------------------------------------------------------------===//
class F1_InstWrapper<bits<6> opcode1, bits<2> opcode2, bits<4> opcode3,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F1_Inst<opcode1, opcode2, opcode3,
            (outs GPR_Class:$rd), (ins GPR_Class:$ra, GPR_Class:$rb),
            opcodeAsm#" $rd, $ra, $rb", [(set GPR_Class:$rd, (opNode GPR_Class:$ra, GPR_Class:$rb))],
            itin>;

def ADD  : F1_InstWrapper<0x38, 0x0, 0x0, "l.add", add>;
def ADDC : F1_InstWrapper<0x38, 0x0, 0x1, "l.addc", addc>;
def SUB  : F1_InstWrapper<0x38, 0x0, 0x2, "l.sub", sub>;
def AND  : F1_InstWrapper<0x38, 0x0, 0x3, "l.and", and>;
def OR   : F1_InstWrapper<0x38, 0x0, 0x4, "l.or", or>;
def XOR  : F1_InstWrapper<0x38, 0x0, 0x5, "l.xor", xor>;

//===----------------------------------------------------------------------===//
// Format 2 - Arithmetic + logic instructions with immediate
//===----------------------------------------------------------------------===//
class F2_InstWrapper<bits<6> opcode,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F2_Inst<opcode,
            (outs GPR_Class:$rd), (ins GPR_Class:$ra, immOp:$imm),
            opcodeAsm#" $rd, $ra, $imm", [(set GPR_Class:$rd, (opNode GPR_Class:$ra, i16imm:$imm))],
            itin>;

def ADDI  : F2_InstWrapper<0x27, "l.addi", add>;
def ADDIC : F2_InstWrapper<0x28, "l.addic", addc>;
def ANDI  : F2_InstWrapper<0x29, "l.andi", and>;
def ORI   : F2_InstWrapper<0x2a, "l.ori", or>;
def XORI  : F2_InstWrapper<0x2b, "l.xori", xor>;

//===----------------------------------------------------------------------===//
// Format 3 - Shift instructions with register
//===----------------------------------------------------------------------===//
class F3_InstWrapper<bits<6> opcode1, bits<4> opcode2, bits<4> opcode3,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F3_Inst<opcode1, opcode2, opcode3,
            (outs GPR_Class:$rd), (ins GPR_Class:$ra, GPR_Class:$rb),
            opcodeAsm#" $rd, $ra, $rb", [(set GPR_Class:$rd, (opNode GPR_Class:$ra, GPR_Class:$rb))],
            itin>;

def SLL  : F3_InstWrapper<0x38, 0x0, 0x8, "l.sll", shl>;
def SRL  : F3_InstWrapper<0x38, 0x1, 0x8, "l.srl", srl>;
def SRA  : F3_InstWrapper<0x38, 0x2, 0x8, "l.sra", sra>;

//===----------------------------------------------------------------------===//
// Format 4 - Shift instructions with immediate
//===----------------------------------------------------------------------===//
class F4_InstWrapper<bits<6> opcode1, bits<2> opcode2,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F4_Inst<opcode1, opcode2,
            (outs GPR_Class:$rd), (ins GPR_Class:$ra, immOp:$imm),
            opcodeAsm#" $rd, $ra, $imm", [(set GPR_Class:$rd, (opNode GPR_Class:$ra, immOp:$imm))],
            itin>;

def SLLI  : F4_InstWrapper<0x2e, 0x0, "l.slli", shl>;
def SRLI  : F4_InstWrapper<0x2e, 0x1, "l.srli", srl>;
def SRAI  : F4_InstWrapper<0x2e, 0x2, "l.srai", sra>;

//===----------------------------------------------------------------------===//
// Format 5  - Jumps and branch instructions with immediate
//===----------------------------------------------------------------------===//
class F5_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F5_Inst<opcode,
            (outs), (ins immOp:$imm),
            opcodeAsm#" $imm", pattern,
            itin>;

def J   : F5_InstWrapper<0x0, "l.j", [(br immOp:$imm)]>;
def JAL : F5_InstWrapper<0x1, "l.jal", [(OpenRisc_Call immOp:$imm)]>;
def BNF : F5_InstWrapper<0x3, "l.bnf", []>;
def BF  : F5_InstWrapper<0x4, "l.bf", []>;

//===----------------------------------------------------------------------===//
// Format 6 - Jumps and branch instructions with register
//===----------------------------------------------------------------------===//
class F6_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F6_Inst<opcode,
            (outs), (ins GPR_Class:$rb),
            opcodeAsm#" $rb", pattern,
            itin>;

def JR   : F6_InstWrapper<0x11, "l.jr", [(brind GPR_Class:$rb)]>;
def JALR : F6_InstWrapper<0x12, "l.jalr", []>;

//===----------------------------------------------------------------------===//
// Format 7 - Store instructions
//===----------------------------------------------------------------------===//
class F7_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F7_Inst<opcode,
            (outs), (ins GPR_Class:$ra, GPR_Class:$rb, immOp:$imm),
            opcodeAsm#" $imm($ra), $rb", pattern,
            itin>;
            
def SW : F7_InstWrapper<0x35, "l.sw", [(store GPR_Class:$rb addrmode_imm11:$addr)]>;
def SB : F7_InstWrapper<0x36, "l.sb", [(truncstorei8 GPR_Class:$rb addrmode_imm11:$addr)]>;
def SH : F7_InstWrapper<0x37, "l.sh", [(truncstorei16 GPR_Class:$rb addrmode_imm11:$addr)]>;

//===----------------------------------------------------------------------===//
// Format 8 - Conditional set-flag instructions
//===----------------------------------------------------------------------===//
class F8_InstWrapper<bits<11> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F8_Inst<opcode,
            (outs), (ins GPR_Class:$ra, GPR_Class:$rb),
            opcodeAsm#" $imm($ra), $rb", pattern,
            itin>;

def SFEQ  : F8_InstWrapper<0x720, "l.sfeq", []>;
def SFNE  : F8_InstWrapper<0x721, "l.sfne", []>;
def SFGTU : F8_InstWrapper<0x722, "l.sfgtu", []>;
def SFGEU : F8_InstWrapper<0x723, "l.sfgeu", []>;
def SFLTU : F8_InstWrapper<0x724, "l.sfltu", []>;
def SFLEU : F8_InstWrapper<0x725, "l.sfleu", []>;

def SFGTS : F8_InstWrapper<0x72a, "l.sfgts", []>;
def SFGES : F8_InstWrapper<0x72b, "l.sfges", []>;
def SFLTS : F8_InstWrapper<0x72c, "l.sflts", []>;
def SFLES : F8_InstWrapper<0x72d, "l.sfles", []>;

//===----------------------------------------------------------------------===//
// Format 9 - "Move Immediate High" instruction
//===----------------------------------------------------------------------===//
class F9_InstWrapper<bits<6> opcode1, bits<1> opcode2,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F9_Inst<opcode1, opcode2,
            (outs GPR_Class:$rd), (ins immOp:$imm),
            opcodeAsm#" $rd, $imm", pattern,
            itin>;

def MOVHI : F9_InstWrapper<0x6, 0x0, "l.movhi", [(set GPR_Class:$rd uimm16hi:$imm)]>;

//===----------------------------------------------------------------------===//
// Format 10 - "No Operation" instruction
//===----------------------------------------------------------------------===//
class F10_InstWrapper<bits<8> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F10_Inst<opcode,
            (outs), (ins immOp:$imm),
            opcodeAsm#" $imm", pattern,
            itin>;

def NOP : F10_InstWrapper<0x15, "l.nop", []>;

//===----------------------------------------------------------------------===//
// Format 11 - "Return From Exception" instruction
//===----------------------------------------------------------------------===//
class F11_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F11_Inst<opcode,
            (outs), (ins),
            opcodeAsm, pattern,
            itin>;

def RFE : F11_InstWrapper<0x9, "l.rfe", []>;

//===----------------------------------------------------------------------===//
// Format 12 - "System Call" instruction
//===----------------------------------------------------------------------===//
class F12_InstWrapper<bits<16> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F12_Inst<opcode,
            (outs), (ins immOp:$imm),
            opcodeAsm#" $imm", pattern,
            itin>;

def SYS : F12_InstWrapper<0x2000, "l.sys", []>;

//===----------------------------------------------------------------------===//
// Format 13 - Load instructions
//===----------------------------------------------------------------------===//
class F13_InstWrapper<bits<6> opcode,
                     string opcodeAsm, SDPatternOperator opNode, ComplexPattern addrOp, Operand memOp,
                     InstrItinClass itin = NoItinerary> :
    F13_Inst<opcode,
            (outs GPR_Class:$rd), (ins GPR_Class:$ra, memOp:$imm),
            opcodeAsm#" $rd, $imm($ra)", [(set GPR_Class:$rd, (opNode addrOp:$addr))],
            itin>;

def LWZ   : F13_InstWrapper<0x21, "l.lwz", load, addrOp, memOp>;
def LWS   : F13_InstWrapper<0x22, "l.lws", load, addrOp, memOp>;
def LBZ   : F13_InstWrapper<0x23, "l.lbz", zextloadi8, addrOp, memOp>;
def LBS   : F13_InstWrapper<0x24, "l.lbs", sextloadi8, addrOp, memOp>>;
def LHZ   : F13_InstWrapper<0x25, "l.lhz", sextloadi16, addrOp, memOp>>;
def LHS   : F13_InstWrapper<0x26, "l.lhs", sextloadi16, addrOp, memOp>>;

//===----------------------------------------------------------------------===//
// Format 14 - Move to/from special purpose registers instructions
//===----------------------------------------------------------------------===//
class F13_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F13_Inst<opcode,
            (outs), (ins GPR_Class:$ra, GPR_Class:$rb, immOp:$imm),
            opcodeAsm#" $ra, $rb, $imm", pattern,
            itin>;

def MFSPR : F14_InstWrapper<0x2d, "l.mfspr", []>;
def MTSPR : F14_InstWrapper<0x30, "l.mtspr", []>;