//===- OR1KInstrInfo.td - OR1KInstruction Descriptions -----*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Operators
//===----------------------------------------------------------------------===//

// Type profiles
def SDT_OR1KCallSeqStart : SDCallSeqStart<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_OR1KCallSeqEnd   : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_OR1KCall         : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;

def SDT_OR1KWrapPtr      : SDTypeProfile<1, 1, [SDTCisSameAs<0, 1>,
                                                SDTCisPtrTy<0>]>;

def SDT_OR1KSelectCC     : SDTypeProfile<1, 5, [SDTCisSameAs<0, 1>,
                                                SDTCisSameAs<2, 3>,
                                                SDTCisVT<5, i32>]>;

def SDT_OR1KBrCC         : SDTypeProfile<0, 4, [SDTCisVT<0, i32>,
                                                SDTCisSameAs<1, 2>,
                                                SDTCisVT<3, OtherVT>]>;

// Node definitions
def or1k_call: SDNode<"OR1KISD::CALL", SDT_OR1KCall,
                      [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

def or1k_ret: SDNode<"OR1KISD::RET", SDTNone,
                     [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def or1k_callseq_start: SDNode<"ISD::CALLSEQ_START", SDT_OR1KCallSeqStart,
                               [SDNPHasChain, SDNPSideEffect, SDNPOutGlue]>;

def or1k_callseq_end  : SDNode<"ISD::CALLSEQ_END",   SDT_OR1KCallSeqEnd,
                               [SDNPHasChain, SDNPSideEffect, SDNPOptInGlue,
                                SDNPOutGlue]>;

def or1k_select_cc: SDNode<"OR1KISD::SELECT_CC", SDT_OR1KSelectCC,
                           [SDNPInGlue]>;

def or1k_brcc
  : SDNode<"OR1KISD::BR_CC", SDT_OR1KBrCC,
           [SDNPHasChain, SDNPInGlue]>;

def or1k_ga_wrapper : SDNode<"OR1KISD::GA_WRAPPER", SDT_OR1KWrapPtr>;

//===----------------------------------------------------------------------===//
// Operands
//===----------------------------------------------------------------------===//

// Immediate operands
class ImmAsmOperand<string prefix, int width, string suffix = "">
: AsmOperandClass {
  let Name = prefix # "Imm" # width # suffix;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", Name);
}

class UImmOp<int bitsNum>
: Operand<i32>, ImmLeaf<i32, "return isUInt<" # bitsNum # ">(Imm);"> {
  let ParserMatchClass = ImmAsmOperand<"U", bitsNum>;
  let EncoderMethod = "getImmOpValue";
  let DecoderMethod = "decodeUImmOperand<" # bitsNum # ">";
  // let OperandType = "OPERAND_UIMM" # bitsNum;
}

class SImmOp<int bitsNum>
: Operand<i32>, ImmLeaf<i32, "return isInt<" # bitsNum # ">(Imm);"> {
  let ParserMatchClass = ImmAsmOperand<"S", bitsNum>;
  let EncoderMethod = "getImmOpValue";
  let DecoderMethod = "decodeSImmOperand<" # bitsNum # ">";
  // let OperandType = "OPERAND_SIMM" # bitsNum;
}

def simm16 : SImmOp<16>;
def uimm6 : UImmOp<6>;
def uimm16 : UImmOp<16>;

def uimm32 : ImmLeaf<i32, "return isUInt<32>(Imm);">;

// Symbolic address operands
def PCRelAsmOperand : AsmOperandClass {
  let Name = "PCRelTarget";
  let ParserMethod = "parsePCRelTarget";
  let PredicateMethod = "isImm";
  let RenderMethod = "addImmOperands";
}

def calltarget : Operand<iPTR> {
  let PrintMethod = "printCallOperand";
  let EncoderMethod = "getImmOpValue";
  let DecoderMethod = "decodeCallOperand";
  let ParserMatchClass = PCRelAsmOperand;
  // let OperandType = "OPERAND_PCREL";
}

def brtarget : Operand<OtherVT> {
  let PrintMethod = "printBranchTarget";
  let EncoderMethod = "getImmOpValue";
  let DecoderMethod = "decodeBranchOperand";
  let ParserMatchClass = PCRelAsmOperand;
  // let OperandType = "OPERAND_PCREL";
}

// Memory operands
def AddrRegImm : ComplexPattern<iPTR, 2, "selectAddrRegImm">;

//===----------------------------------------------------------------------===//
// Instructions
//===----------------------------------------------------------------------===//
class OR1K_Inst<dag outs, dag ins,
                string asmStr, list<dag> pattern = []>
: Instruction {
  field bits<32> Inst;
  let Namespace = "OR1K";
  let Size = 4;
  dag OutOperandList = outs;
  dag InOperandList = ins;
  let AsmString = asmStr;
  let Pattern = pattern;
}

//===----------------------------------------------------------------------===//
// ALU_RR
//===----------------------------------------------------------------------===//
class ALU_RR_Inst<bits<6> opcode1, bits<2> opcode2, bits<4> opcode3,
                  string opcodeAsm, SDPatternOperator opNode>
: OR1K_Inst<(outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
            opcodeAsm#" $rd, $ra, $rb",
            [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))]> {
  bits<5> rd;
  bits<5> ra;
  bits<5> rb;
  bits<1> reserved1 = 0;
  bits<4> reserved2 = 0;

  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10}    = reserved1;
  let Inst{9-8}   = opcode2;
  let Inst{7-4}   = reserved2;
  let Inst{3-0}   = opcode3;
}

def ADD  : ALU_RR_Inst<0x38, 0x0, 0x0, "l.add", add>;
def ADDC : ALU_RR_Inst<0x38, 0x0, 0x1, "l.addc", addc>;
def SUB  : ALU_RR_Inst<0x38, 0x0, 0x2, "l.sub", sub>;
def AND  : ALU_RR_Inst<0x38, 0x0, 0x3, "l.and", and>;
def OR   : ALU_RR_Inst<0x38, 0x0, 0x4, "l.or", or>;
def XOR  : ALU_RR_Inst<0x38, 0x0, 0x5, "l.xor", xor>;

//===----------------------------------------------------------------------===//
// ALU_RI
//===----------------------------------------------------------------------===//
class ALU_RI_Inst<bits<6> opcode,
                  string opcodeAsm, SDPatternOperator opNode,
                  ImmLeaf immOp>
: OR1K_Inst<(outs GPR:$rd), (ins GPR:$ra, immOp:$imm),
            opcodeAsm#" $rd, $ra, $imm",
            [(set GPR:$rd, (opNode GPR:$ra, immOp:$imm))]> {
  bits<5>  rd;
  bits<5>  ra;
  bits<16> imm;

  let Inst{31-26} = opcode;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-0}  = imm;
}

def ADDI  : ALU_RI_Inst<0x27, "l.addi", add, simm16>;
def ADDIC : ALU_RI_Inst<0x28, "l.addic", addc, simm16>;
def ANDI  : ALU_RI_Inst<0x29, "l.andi", and, uimm16>;
def ORI   : ALU_RI_Inst<0x2a, "l.ori", or, uimm16>;
def XORI  : ALU_RI_Inst<0x2b, "l.xori", xor, simm16>;

//===----------------------------------------------------------------------===//
// SHIFT_RR
//===----------------------------------------------------------------------===//
class SHIFT_RR_Inst<bits<6> opcode1, bits<4> opcode2, bits<4> opcode3,
                    string opcodeAsm, SDPatternOperator opNode>
: OR1K_Inst<(outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
            opcodeAsm#" $rd, $ra, $rb",
            [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))]> {
  bits<5> rd;
  bits<5> ra;
  bits<5> rb;
  bits<1> reserved1 = 0;
  bits<2> reserved2 = 0;

  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10}    = reserved1;
  let Inst{9-6}   = opcode2;
  let Inst{5-4}   = reserved2;
  let Inst{3-0}   = opcode3;
}

def SLL : SHIFT_RR_Inst<0x38, 0x0, 0x8, "l.sll", shl>;
def SRL : SHIFT_RR_Inst<0x38, 0x1, 0x8, "l.srl", srl>;
def SRA : SHIFT_RR_Inst<0x38, 0x2, 0x8, "l.sra", sra>;

//===----------------------------------------------------------------------===//
// SHIFT_RI
//===----------------------------------------------------------------------===//
class SHIFT_RI_Inst<bits<6> opcode1, bits<2> opcode2,
                    string opcodeAsm, SDPatternOperator opNode>
: OR1K_Inst<(outs GPR:$rd), (ins GPR:$ra, uimm6:$imm),
            opcodeAsm#" $rd, $ra, $imm",
            [(set GPR:$rd, (opNode GPR:$ra, uimm6:$imm))]> {
  bits<5> rd;
  bits<5> ra;
  bits<8> reserved = 0;
  bits<6> imm;

  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-8}  = reserved;
  let Inst{7-6}   = opcode2;
  let Inst{5-0}   = imm;
}

def SLLI : SHIFT_RI_Inst<0x2e, 0x0, "l.slli", shl>;
def SRLI : SHIFT_RI_Inst<0x2e, 0x1, "l.srli", srl>;
def SRAI : SHIFT_RI_Inst<0x2e, 0x2, "l.srai", sra>;

//===----------------------------------------------------------------------===//
// BRANCH_R
//===----------------------------------------------------------------------===//
class BRANCH_R_Inst<bits<6> opcode, string opcodeAsm, list<dag> pattern>
: OR1K_Inst<(outs), (ins GPR:$rb), opcodeAsm#" $rb", pattern> {
  bits<10> reserved1 = 0;
  bits<5>  rb;
  bits<11> reserved2 = 0;

  let Inst{31-26} = opcode;
  let Inst{25-16} = reserved1;
  let Inst{15-11} = rb;
  let Inst{10-0}  = reserved2;
}

def JR   : BRANCH_R_Inst<0x11, "l.jr", [(brind GPR:$rb)]> {
  let isBranch = 1;
  let isTerminator = 1;
  let isIndirectBranch = 1;
  let isBarrier = 1;
}
def JALR : BRANCH_R_Inst<0x12, "l.jalr", [(or1k_call GPR:$rb)]> {
  let isCall = 1;
  let Defs = [R9];
}

//===----------------------------------------------------------------------===//
// BRANCH_I
//===----------------------------------------------------------------------===//
class BRANCH_I_Inst<bits<6> opcode, string opcodeAsm, dag ins,
                    list<dag> pattern = []>
  : OR1K_Inst<(outs), ins, opcodeAsm#" $imm", pattern> {
  bits<26> imm;

  let Inst{31-26} = opcode;
  let Inst{25-0}  = imm;
}

def J   : BRANCH_I_Inst<0x0, "l.j", (ins brtarget:$imm), [(br bb:$imm)]> {
  let isBranch     = 1;
  let isTerminator = 1;
  let isBarrier    = 1;
}

def JAL : BRANCH_I_Inst<0x1, "l.jal", (ins calltarget:$imm)> {
  let isCall = 1;
  let Defs = [R9];
}

let Uses = [SR_FFlag], isBranch = 1, isTerminator = 1 in {
def BNF : BRANCH_I_Inst<0x3, "l.bnf", (ins brtarget:$imm)>;
def BF  : BRANCH_I_Inst<0x4, "l.bf", (ins brtarget:$imm)>;
} // Uses = [SR_FFlag]

//===----------------------------------------------------------------------===//
// LOAD
//===----------------------------------------------------------------------===//
class LOAD_Inst<bits<6> opcode, string opcodeAsm, SDPatternOperator opNode>
    : OR1K_Inst<(outs GPR:$rd), (ins simm16:$imm, GPR:$ra),
                opcodeAsm#" $rd, ${imm}(${ra})",
                [(set GPR:$rd, (opNode (AddrRegImm GPR:$ra, simm16:$imm)))]> {
  bits<5>  rd;
  bits<5>  ra;
  bits<16> imm;

  let Inst{31-26} = opcode;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-0}  = imm;

  let mayLoad = 1;
}

def LWZ : LOAD_Inst<0x21, "l.lwz", load>;
def LWS : LOAD_Inst<0x22, "l.lws", load>;
def LBZ : LOAD_Inst<0x23, "l.lbz", zextloadi8>;
def LBS : LOAD_Inst<0x24, "l.lbs", sextloadi8>;
def LHZ : LOAD_Inst<0x25, "l.lhz", zextloadi16>;
def LHS : LOAD_Inst<0x26, "l.lhs", sextloadi16>;

//===----------------------------------------------------------------------===//
// STORE
//===----------------------------------------------------------------------===//
class STORE_Inst<bits<6> opcode, string opcodeAsm, SDPatternOperator opNode>
    : OR1K_Inst<(outs), (ins simm16:$imm, GPR:$ra, GPR:$rb),
                opcodeAsm#" ${imm}(${ra}), $rb",
                [(opNode GPR:$rb, (AddrRegImm GPR:$ra, simm16:$imm))]> {
  bits<16> imm;
  bits<5>  ra;
  bits<5>  rb;

  let Inst{31-26} = opcode;
  let Inst{25-21} = imm{15-11};
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10-0}  = imm{10-0};
}

def SW : STORE_Inst<0x35, "l.sw", store>;
def SB : STORE_Inst<0x36, "l.sb", truncstorei8>;
def SH : STORE_Inst<0x37, "l.sh", truncstorei16>;

//===----------------------------------------------------------------------===//
// SETFLAG
//===----------------------------------------------------------------------===//
class SETFLAG_Inst<bits<11> opcode, string opcodeAsm>
    : OR1K_Inst<(outs), (ins GPR:$ra, GPR:$rb),
                opcodeAsm#" $ra, $rb"> {
  bits<5>  ra;
  bits<5>  rb;
  bits<11> reserved = 0;

  let Inst{31-21} = opcode;
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10-0}  = reserved;

  let Defs = [SR_FFlag];
}

def SFEQ  : SETFLAG_Inst<0x720, "l.sfeq">;
def SFNE  : SETFLAG_Inst<0x721, "l.sfne">;
def SFGTU : SETFLAG_Inst<0x722, "l.sfgtu">;
def SFGEU : SETFLAG_Inst<0x723, "l.sfgeu">;
def SFLTU : SETFLAG_Inst<0x724, "l.sfltu">;
def SFLEU : SETFLAG_Inst<0x725, "l.sfleu">;
def SFGTS : SETFLAG_Inst<0x72a, "l.sfgts">;
def SFGES : SETFLAG_Inst<0x72b, "l.sfges">;
def SFLTS : SETFLAG_Inst<0x72c, "l.sflts">;
def SFLES : SETFLAG_Inst<0x72d, "l.sfles">;

//===----------------------------------------------------------------------===//
// MOVHI
//===----------------------------------------------------------------------===//
class MOVHI_Inst<bits<6> opcode1, bits<1> opcode2,
                 dag outs, dag ins,
                 string asmStr>
    : OR1K_Inst<outs, ins, asmStr> {

  bits<5>  rd;
  bits<4>  reserved = 0;
  bits<16> imm;

  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-17} = reserved;
  let Inst{16}    = opcode2;
  let Inst{15-0}  = imm;
}

def MOVHI : MOVHI_Inst<0x6, 0x0,
                       (outs GPR:$rd), (ins uimm16:$imm),
                       "l.movhi $rd, $imm">;

//===----------------------------------------------------------------------===//
// NOP
//===----------------------------------------------------------------------===//
class NOP_Inst<bits<8> opcode,
               dag outs, dag ins,
               string asmStr>
    : OR1K_Inst<outs, ins, asmStr> {

  bits<8>  reserved = 0;
  bits<16> imm;

  let Inst{31-24} = opcode;
  let Inst{23-16} = reserved;
  let Inst{15-0}  = imm;
}

def NOP : NOP_Inst<0x15,
                   (outs), (ins uimm16:$imm),
                   "l.nop $imm">;

//===----------------------------------------------------------------------===//
// RFE
//===----------------------------------------------------------------------===//
class RFE_Inst<bits<6> opcode,
               dag outs, dag ins,
               string asmStr>
    : OR1K_Inst<outs, ins, asmStr> {
  bits<26> reserved = 0;

  let Inst{31-26} = opcode;
  let Inst{25-0}  = reserved;
}

def RFE : RFE_Inst<0x9,
                   (outs), (ins),
                   "l.rfe">;

//===----------------------------------------------------------------------===//
// SYS
//===----------------------------------------------------------------------===//
class SYS_Inst<bits<16> opcode,
               dag outs, dag ins,
               string asmStr>
    : OR1K_Inst<outs, ins, asmStr> {
  bits<16> imm;

  let Inst{31-16} = opcode;
  let Inst{15-0}  = imm;
}

def SYS : SYS_Inst<0x2000,
                   (outs), (ins uimm16:$imm),
                   "l.sys $imm">;

//===----------------------------------------------------------------------===//
// Pseudo
//===----------------------------------------------------------------------===//
class Pseudo_Inst<dag outs, dag ins,
                  string asmstr, list<dag> pattern>
    : OR1K_Inst<outs, ins, asmstr, pattern> {
  let isPseudo = 1;
  let isCodeGenOnly = 1;
}

let Defs = [R1], Uses = [R1] in {
def ADJCALLSTACKDOWN : Pseudo_Inst<(outs), (ins i32imm:$ra, i32imm:$rb),
                                   "#ADJCALLSTACKDOWN",
                                   [(or1k_callseq_start timm:$ra, timm:$rb)]>;

def ADJCALLSTACKUP   : Pseudo_Inst<(outs), (ins i32imm:$ra, i32imm:$rb),
                                   "#ADJCALLSTACKUP",
                                   [(or1k_callseq_end timm:$ra, timm:$rb)]>;
} // Defs = [R1], Uses = [R1]

let usesCustomInserter = 1 in {
def SELECT_CC
    : Pseudo_Inst<(outs GPR:$rd),
                  (ins GPR:$ra, GPR:$rb, GPR:$T, GPR:$F, i32imm:$cc),
                  "#SELECT_CC",
                  [(set GPR:$rd, (or1k_select_cc GPR:$ra, GPR:$rb, GPR:$T,
                                  GPR:$F, i32imm:$cc))]>;

let isBranch = 1, isTerminator = 1 in {
def BRCC_PSEUDO
    : Pseudo_Inst<(outs),
                  (ins GPR:$ra, GPR:$rb, i32imm:$cc, brtarget:$t),
                  "#BR_CC_PSEUDO",
                  [(or1k_brcc i32imm:$cc, GPR:$ra, GPR:$rb, bb:$t)]>;
} // isBranch = 1, isTerminator = 1
} // usesCustomInserter = 1

//===----------------------------------------------------------------------===//
// Patterns
//===----------------------------------------------------------------------===//

// Returns
def : Pat<(or1k_ret), (JR R9)>;

// Calls
def : Pat<(or1k_call tglobaladdr:$imm),  (JAL calltarget:$imm)>;
def : Pat<(or1k_call texternalsym:$imm), (JAL calltarget:$imm)>;

// Symbol moves
// def : Pat<(or1k_ga_wrapper tglobaladdr:$addr),
//          (ORI (MOVHI (tglobaladdr:$addr)), (tglobaladdr:$addr))>;
// def : Pat<(or1k_ga_wrapper texternalsym:$addr),
//          (ORI (MOVHI (texternalsym:$addr)),
//          (texternalsym:$addr))>;

// Unsigned 16-bit moves
def : Pat<(uimm16:$imm), (ORI R0, uimm16:$imm)>;

// Unsigned 32-bit bit moves
def imm_lo_zero : ImmLeaf<i32, "return (Imm & 0xFFFF) == 0;">;

def uimm16_hi_XFORM : SDNodeXForm<imm, [{
  uint64_t V = (uint64_t)N->getZExtValue();
  return CurDAG->getTargetConstant((V >> 16) & 0xFFFF, SDLoc(N), MVT::i32);
}]>;

def uimm16_lo_XFORM : SDNodeXForm<imm, [{
  uint64_t V = (uint64_t)N->getZExtValue();
  return CurDAG->getTargetConstant(V & 0xFFFF, SDLoc(N), MVT::i32);
}]>;

def : Pat<(imm_lo_zero:$C), (MOVHI (uimm16_hi_XFORM imm_lo_zero:$C))>;
def : Pat<(uimm32:$imm), (ORI (MOVHI (uimm16_hi_XFORM uimm32:$imm)),
                              (uimm16_lo_XFORM uimm32:$imm))>;
