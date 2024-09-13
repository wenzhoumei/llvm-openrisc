//===- OpenRiscInstrInfo.td - OpenRiscInstruction Descriptions -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file describes the OpenRisc instructions in TableGen format.
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
                     string opcodeAsm, SDPatternOperator opNode, Immediate immOp,
                     InstrItinClass itin = NoItinerary> :
    F2_Inst<opcode,
            (outs GPR:$rd), (ins GPR:$ra, immOp:$imm),
            opcodeAsm#" $rd, $ra, $imm", [(set GPR:$rd, (opNode GPR:$ra, immOp:$imm))],
            itin>;

def ADDI  : F2_InstWrapper<0x27, "l.addi", add, simm16>;
def ADDIC : F2_InstWrapper<0x28, "l.addic", addc, simm16>;
def ANDI  : F2_InstWrapper<0x29, "l.andi", and, uimm16>;
def ORI   : F2_InstWrapper<0x2a, "l.ori", or, uimm16>;
def XORI  : F2_InstWrapper<0x2b, "l.xori", xor, uimm16>;

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
let isBranch = 1, isTerminator = 1 in {
def JR   : F5_InstWrapper<0x11, "l.jr", [(brind GPR:$rb)]>;
def JALR : F5_InstWrapper<0x12, "l.jalr", [(OpenRisc_call GPR:$rb)]>;
}
//===----------------------------------------------------------------------===//
// Format 6  - Jumps and branch instructions with immediate
//===----------------------------------------------------------------------===//
class F6_InstWrapper<bits<6> opcode,
                     string opcodeAsm, list<dag> pattern, dag ins,
                     InstrItinClass itin = NoItinerary> :
    F6_Inst<opcode,
            (outs), ins,
            opcodeAsm#" $imm", pattern,
            itin>;

let isBranch = 1, isTerminator = 1 in {
def J   : F6_InstWrapper<0x0, "l.j", [(br bb:$imm)], (ins brtarget:$imm)>;
def JAL : F6_InstWrapper<0x1, "l.jal", [], (ins calltarget:$imm)>;
def BNF : F6_InstWrapper<0x3, "l.bnf", [], (ins SR:$sr, brtarget:$imm)>;
def BF  : F6_InstWrapper<0x4, "l.bf", [], (ins SR:$sr, brtarget:$imm)>;
}


//===----------------------------------------------------------------------===//
// Format 7 - Load instructions
//===----------------------------------------------------------------------===//
class F7_InstWrapper<bits<6> opcode,
                     string opcodeAsm, SDPatternOperator opNode,
                     InstrItinClass itin = NoItinerary> :
    F7_Inst<opcode,
            (outs GPR:$rd), (ins simm16:$imm, GPR:$ra),
            opcodeAsm#" $rd, ${imm}(${ra})", [(set GPR:$rd, (opNode (addrimm GPR:$ra, simm16:$imm)))], // https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/RISCV/RISCVInstrInfo.td#L1708
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
            (outs), (ins simm16:$imm, GPR:$ra, GPR:$rb),
            opcodeAsm#" ${imm}(${ra}), $rb", [(opNode GPR:$rb, (addrimm GPR:$ra, simm16:$imm))],
            itin>;
            
def SW : F8_InstWrapper<0x35, "l.sw", store>;
def SB : F8_InstWrapper<0x36, "l.sb", truncstorei8>;
def SH : F8_InstWrapper<0x37, "l.sh", truncstorei16>;

//===----------------------------------------------------------------------===//
// Format 9 - Conditional set-flag instructions
//===----------------------------------------------------------------------===//
class F9_InstWrapper<bits<11> opcode,
                     string opcodeAsm, CondCode cc,
                     InstrItinClass itin = NoItinerary> :
    F9_Inst<opcode,
            (outs SR:$sr), (ins GPR:$ra, GPR:$rb),
            opcodeAsm#" $ra, $rb", [/*(set SR:$sr, (setcc GPR:$ra, GPR:$rb, cc))*/],
            itin>;

def SFEQ  : F9_InstWrapper<0x720, "l.sfeq", SETEQ>;
def SFNE  : F9_InstWrapper<0x721, "l.sfne", SETNE>;
def SFGTU : F9_InstWrapper<0x722, "l.sfgtu", SETUGT>;
def SFGEU : F9_InstWrapper<0x723, "l.sfgeu", SETUGE>;
def SFLTU : F9_InstWrapper<0x724, "l.sfltu", SETULT>;
def SFLEU : F9_InstWrapper<0x725, "l.sfleu", SETULE>;
def SFGTS : F9_InstWrapper<0x72a, "l.sfgts", SETGT>;
def SFGES : F9_InstWrapper<0x72b, "l.sfges", SETGE>;
def SFLTS : F9_InstWrapper<0x72c, "l.sflts", SETLT>;
def SFLES : F9_InstWrapper<0x72d, "l.sfles", SETLE>;

//===----------------------------------------------------------------------===//
// Format 10 - "Move Immediate High" instruction
//===----------------------------------------------------------------------===//
class F10_InstWrapper<bits<6> opcode1, bits<1> opcode2,
                     string opcodeAsm, list<dag> pattern,
                     InstrItinClass itin = NoItinerary> :
    F10_Inst<opcode1, opcode2,
            (outs GPR:$rd), (ins imm16hi:$imm),
            opcodeAsm#" $rd, $imm", pattern,
            itin>;

def MOVHI : F10_InstWrapper<0x6, 0x0, "l.movhi", [(set GPR:$rd, imm16hi:$imm)]>;

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
// Calls
//===----------------------------------------------------------------------===//
let isReturn = 1, isTerminator = 1, isBarrier = 1, Uses = [R9] in {
def RET : F5_Inst<0x11, (outs), (ins),
                "l.jr lr", [(OpenRisc_ret)]> {
        let rb = 0x9;
}
}

//def: Pat<(brcond (seteq SR:$sr, 0), bb:$imm), (BNF SR:$sr, bb:$imm)>;
//def: Pat<(brcond (seteq SR:$sr, 1), bb:$imm), (BF SR:$sr, bb:$imm)>;

def : Pat<(OpenRisc_call tglobaladdr:$imm), (JAL $imm)>;
def : Pat<(OpenRisc_call texternalsym:$imm), (JAL $imm)>;

//===----------------------------------------------------------------------===//
// Stack allocation
//===----------------------------------------------------------------------===//
let Defs = [R1], Uses = [R1] in {
def ADJCALLSTACKDOWN : Pseudo_Inst<(outs), (ins i32imm:$ra, i32imm:$rb),
                                   "#ADJCALLSTACKDOWN",
                                   [(OpenRisc_callseq_start timm:$ra, timm:$rb)]>;

def ADJCALLSTACKUP   : Pseudo_Inst<(outs), (ins i32imm:$ra, i32imm:$rb),
                                   "#ADJCALLSTACKUP",
                                   [(OpenRisc_callseq_end timm:$ra, timm:$rb)]>;
}

//===----------------------------------------------------------------------===//
// XFORM
//===----------------------------------------------------------------------===//
def imm16_hi_XFORM : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((int)N->getSExtValue() & (~0xFFFF), SDLoc(N), MVT::i32);
}]>;


def imm16_lo_XFORM : SDNodeXForm<imm, [{
  return CurDAG->getTargetConstant((int)N->getSExtValue() & 0xFFFF, SDLoc(N), MVT::i32);
}]>;

//===----------------------------------------------------------------------===//
// Other move patterns
//===----------------------------------------------------------------------===//
def MOVLO_SYM : F2_Inst<0x2a, 
                        (outs GPR:$rd), (ins GPR:$ra, losymtarget:$imm),
                        "ori $rd, $ra, $imm", []>;
def MOVHI_SYM : F10_Inst<0x6, 0x0,
                        (outs GPR:$rd), (ins hisymtarget:$imm),
                         "l.movhi $rd, $imm", []>;

// Symbol moves
def : Pat<(OpenRisc_ga_wrapper tglobaladdr:$addr), (MOVLO_SYM (MOVHI_SYM (tglobaladdr:$addr)), (tglobaladdr:$addr))>;
def : Pat<(OpenRisc_ga_wrapper texternalsym:$addr), (MOVLO_SYM (MOVHI_SYM (texternalsym:$addr)), (texternalsym:$addr))>;

// 16 bit immediate moves
def : Pat<(imm16:$imm), (ORI R0, imm16:$imm)>;

// 32-bit bit moves that can't be represented by l.movhi
def : Pat<(imm32:$imm), (ORI (MOVHI (imm16_hi_XFORM imm32:$imm)), (imm16_lo_XFORM imm32:$imm))>;

//===----------------------------------------------------------------------===//
// Other branch patterns
//===----------------------------------------------------------------------===//
// Conditional (brcc) branches
multiclass BrccPat<CondCode cc, F9_InstWrapper INST> {
  def : Pat<(brcc cc, GPR:$ra, GPR:$rb, bb:$imm),
            (BF (INST GPR:$ra, GPR:$rb), bb:$imm)>;
}

defm : BrccPat<SETEQ, SFEQ>;
defm : BrccPat<SETNE, SFNE>;
defm : BrccPat<SETUGT, SFGTU>;
defm : BrccPat<SETUGE, SFGEU>;
defm : BrccPat<SETULT, SFLTU>;
defm : BrccPat<SETULE, SFLEU>;
defm : BrccPat<SETGT, SFGTS>;
defm : BrccPat<SETGE, SFGES>;
defm : BrccPat<SETLT, SFLTS>;
defm : BrccPat<SETLE, SFLES>;

//===----------------------------------------------------------------------===//
// Other
//===----------------------------------------------------------------------===//
let usesCustomInserter = 1 in {
def SELECT_CC
: Pseudo_Inst<(outs GPR:$rd), (ins GPR:$ra, GPR:$rb, GPR:$T, GPR:$F, imm32:$cc),
              "; SELECT_CC PSEUDO!",
              [(set GPR:$rd, (OpenRisc_select_cc GPR:$ra, GPR:$rb, GPR:$T, GPR:$F, imm32:$cc))]>;
}