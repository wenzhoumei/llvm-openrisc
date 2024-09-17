//===- OpenRiscOperands.td - OpenRisc instruction operands -------*- tblgen-*--===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//===----------------------------------------------------------------------===//
// Immediate operands
//===----------------------------------------------------------------------===//
class ImmAsmOperand<string name> : AsmOperandClass {
  let Name = name;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", name);
}

class Immediate<ValueType vt, code pred, string name>
  : Operand<vt>, ImmLeaf<vt, pred> {
  let PrintMethod = "print"#name#"";
  let EncoderMethod = "get"#name#"OpValue";
  let DecoderMethod = "decode"#name;
  let ParserMatchClass = !cast<AsmOperandClass>(name#"_AsmOperand");
}

def SImm16_AsmOperand : ImmAsmOperand<"SImm16">;
def simm16 : Immediate<i32, [{ return isInt<16>(Imm); }], "SImm16">;

def UImm5_AsmOperand : ImmAsmOperand<"UImm5">;
def uimm5 : Immediate<i32, [{ return isUInt<5>(Imm); }], "UImm5">;

def UImm16_AsmOperand : ImmAsmOperand<"UImm16">;
def uimm16 : Immediate<i32, [{ return isUInt<16>(Imm); }], "UImm16">;

def Imm16_AsmOperand : ImmAsmOperand<"Imm16">;
def imm16 : Immediate<i32, [{ return isInt<16>(Imm) || isUInt<16>(Imm); }], "Imm16">;

def Imm32_AsmOperand : ImmAsmOperand<"Imm32">;
def imm32 : Immediate<i32, [{ return isInt<32>(Imm) || isUInt<32>(Imm); }], "Imm32">;

def Imm16High_AsmOperand : ImmAsmOperand<"Imm16High">;
def imm16hi : Immediate<i32, [{ return (Imm & 0xFFFF) == 0; }], "Imm16High">;

//===----------------------------------------------------------------------===//
// Symbolic address operands
//===----------------------------------------------------------------------===//
def PCRelTargetAsmOperand : AsmOperandClass {
  let Name = "PCRelTarget";
  let ParserMethod = "parsePCRelTarget";
  let PredicateMethod = "isImm";
  let RenderMethod = "addImmOperands";
}

def brtarget : Operand<OtherVT> {
  let PrintMethod = "printBranchTarget";
  let EncoderMethod = "getBranchTargetEncoding";
  let DecoderMethod = "decodeBranchOperand";
  let ParserMatchClass = PCRelTargetAsmOperand;
}

def calltarget : Operand<iPTR> {
  let PrintMethod = "printCallOperand";
  let EncoderMethod = "getCallEncoding";
  let DecoderMethod = "decodeCallOperand";
  let ParserMatchClass = PCRelTargetAsmOperand;
}

//===----------------------------------------------------------------------===//
// Other
//===----------------------------------------------------------------------===//
def addrimm : ComplexPattern<iPTR, 2, "selectAddrRegImm">;