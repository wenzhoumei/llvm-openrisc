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
            (outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
            opcodeAsm#" $rd, $ra, $rb", [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))],
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
            (outs GPR:$rd), (ins GPR:$ra, i16imm:$imm),
            opcodeAsm#" $rd, $ra, $imm", [(set GPR:$rd, (opNode GPR:$ra, i16imm:$imm))],
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
            (outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
            opcodeAsm#" $rd, $ra, $rb", [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))],
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
            (outs GPR:$rd), (ins GPR:$ra, uimm5:$imm),
            opcodeAsm#" $rd, $ra, $imm", [(set GPR:$rd, (opNode GPR:$ra, uimm5:$imm))],
            itin>;

def SLLI  : F4_InstWrapper<0x2e, 0x0, "l.slli", shl>;
def SRLI  : F4_InstWrapper<0x2e, 0x1, "l.srli", srl>;
def SRAI  : F4_InstWrapper<0x2e, 0x2, "l.srai", sra>;

//===----------------------------------------------------------------------===//
// Format 5 - Jumps and branch instructions with register
//===----------------------------------------------------------------------===//
class F5_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F5_Inst<opcode,
            (outs), (ins GPR:$rb),
            opcodeAsm#" $rb", pattern,
            itin>;

def JR   : F5_InstWrapper<0x11, "l.jr", [(brind GPR:$rb)]>;
def JALR : F5_InstWrapper<0x12, "l.jalr", [(OpenRisc_call GPR:$rb)]>;

//===----------------------------------------------------------------------===//
// Format 6  - Jumps and branch instructions with immediate
//===----------------------------------------------------------------------===//
class F6_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F6_Inst<opcode,
            (outs), (ins i32imm:$imm),
            opcodeAsm#" $imm", pattern,
            itin>;

def J   : F6_InstWrapper<0x0, "l.j", [(br tglobaladdr:$imm)]>;
def JAL : F6_InstWrapper<0x1, "l.jal", [(OpenRisc_call tglobaladdr:$imm)]>;
def BNF : F6_InstWrapper<0x3, "l.bnf", []>;
def BF  : F6_InstWrapper<0x4, "l.bf", []>;

//===----------------------------------------------------------------------===//
// Format 7 - Load instructions
//===----------------------------------------------------------------------===//
class F7_InstWrapper<bits<6> opcode,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F7_Inst<opcode,
            (outs GPR:$rd), (ins memri:$addr),
            opcodeAsm#" $rd, $imm($ra)", [(set GPR:$rd, (opNode addrimm:$addr))],
            itin>;

def LWZ   : F7_InstWrapper<0x21, "l.lwz", load>;
def LWS   : F7_InstWrapper<0x22, "l.lws", load>;
def LBZ   : F7_InstWrapper<0x23, "l.lbz", zextloadi8>;
def LBS   : F7_InstWrapper<0x24, "l.lbs", sextloadi8>;
def LHZ   : F7_InstWrapper<0x25, "l.lhz", sextloadi16>;
def LHS   : F7_InstWrapper<0x26, "l.lhs", sextloadi16>;

//===----------------------------------------------------------------------===//
// Format 8 - Store instructions
//===----------------------------------------------------------------------===//
class F8_InstWrapper<bits<6> opcode,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F8_Inst<opcode,
            (outs), (ins GPR:$ra, memri:$addr),
            opcodeAsm#" $imm($ra), $rb", [(opNode GPR:$rb, addrimm:$addr)],
            itin>;
            
def SW : F8_InstWrapper<0x35, "l.sw", store>;
def SB : F8_InstWrapper<0x36, "l.sb", truncstorei8>;
def SH : F8_InstWrapper<0x37, "l.sh", truncstorei16>;

//===----------------------------------------------------------------------===//
// Format 9 - Conditional set-flag instructions
//===----------------------------------------------------------------------===//
class F9_InstWrapper<bits<11> opcode,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F9_Inst<opcode,
            (outs SR:$sr), (ins GPR:$ra, GPR:$rb),
            opcodeAsm#" $ra, $rb", [(set SR:$sr, (opNode GPR:$ra, GPR:$rb))],
            itin>;

def SFEQ  : F9_InstWrapper<0x720, "l.sfeq", seteq>;
def SFNE  : F9_InstWrapper<0x721, "l.sfne", setne>;
def SFGTU : F9_InstWrapper<0x722, "l.sfgtu", setugt>;
def SFGEU : F9_InstWrapper<0x723, "l.sfgeu", setuge>;
def SFLTU : F9_InstWrapper<0x724, "l.sfltu", setult>;
def SFLEU : F9_InstWrapper<0x725, "l.sfleu", setule>;
def SFGTS : F9_InstWrapper<0x72a, "l.sfgts", setgt>;
def SFGES : F9_InstWrapper<0x72b, "l.sfges", setge>;
def SFLTS : F9_InstWrapper<0x72c, "l.sflts", setlt>;
def SFLES : F9_InstWrapper<0x72d, "l.sfles", setle>;

//===----------------------------------------------------------------------===//
// Format 10 - "Move Immediate High" instruction
//===----------------------------------------------------------------------===//
class F10_InstWrapper<bits<6> opcode1, bits<1> opcode2,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F10_Inst<opcode1, opcode2,
            (outs GPR:$rd), (ins uimm16hi:$imm),
            opcodeAsm#" $rd, $imm", pattern,
            itin>;

def MOVHI : F10_InstWrapper<0x6, 0x0, "l.movhi", [(set GPR:$rd, uimm16hi:$imm)]>;

//===----------------------------------------------------------------------===//
// Format 11 - Move to/from special purpose registers instructions
//===----------------------------------------------------------------------===//
class F11_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F11_Inst<opcode,
            (outs), (ins GPR:$ra, GPR:$rb, i16imm:$imm),
            opcodeAsm#" $ra, $rb, $imm", pattern,
            itin>;

def MFSPR : F11_InstWrapper<0x2d, "l.mfspr", []>;
def MTSPR : F11_InstWrapper<0x30, "l.mtspr", []>;

//===----------------------------------------------------------------------===//
// Format 12 - "No Operation" instruction
//===----------------------------------------------------------------------===//
class F12_InstWrapper<bits<8> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F12_Inst<opcode,
            (outs), (ins i16imm:$imm),
            opcodeAsm#" $imm", pattern,
            itin>;

def NOP : F12_InstWrapper<0x15, "l.nop", []>;

//===----------------------------------------------------------------------===//
// Format 13 - "Return From Exception" instruction
//===----------------------------------------------------------------------===//
class F13_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F13_Inst<opcode,
            (outs), (ins),
            opcodeAsm, pattern,
            itin>;

def RFE : F13_InstWrapper<0x9, "l.rfe", []>;

//===----------------------------------------------------------------------===//
// Format 14 - "System Call" instruction
//===----------------------------------------------------------------------===//
class F14_InstWrapper<bits<16> opcode,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F14_Inst<opcode,
            (outs), (ins i16imm:$imm),
            opcodeAsm#" $imm", pattern,
            itin>;

def SYS : F14_InstWrapper<0x2000, "l.sys", []>;

//===----------------------------------------------------------------------===//
// Stack allocation
//===----------------------------------------------------------------------===//

// ADJCALLSTACKDOWN/UP implicitly use/def SP because they may be expanded into
// a stack adjustment and the codegen must know that they may modify the stack
// pointer before prolog-epilog rewriting occurs.
let Defs = [R1], Uses = [R1] in {
  def ADJCALLSTACKDOWN : Pseudo_Inst<(outs), (ins i32imm:$ra, i32imm:$rb),
                               "#ADJCALLSTACKDOWN",
                               [(OpenRisc_callseq_start timm:$ra, timm:$rb)]>;
  def ADJCALLSTACKUP   : Pseudo_Inst<(outs), (ins i32imm:$amt1, i32imm:$amt2),
                               "#ADJCALLSTACKUP",
                               [(OpenRisc_callseq_end timm:$ra, timm:$rb)]>;
}