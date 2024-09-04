//===- OpenRiscOperands.td - OpenRisc instruction operands -------*- tblgen-*--===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Xtensa/XtensaOperands.td#L11
class ImmAsmOperand<string name> : AsmOperandClass {
  let Name = name;
  let RenderMethod = "addImmOperands";
  let DiagnosticType = !strconcat("Invalid", name);
}


/// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Xtensa/XtensaOperands.td#L18
class Immediate<ValueType vt, code pred, string asmOp>
  : Operand<vt>, ImmLeaf<vt, pred> {
    let PrintMethod = "print"#asmOp;
    let ParserMatchClass = !cast<AsmOperandClass>(asmOp);
}

// Immediate operations
def UImm16High_AsmOperand : ImmAsmOperand<"UImm16High">;
def uimm16hi : Immediate<i32, [{ return (Imm & 0xFFFF) == 0; }], "UImm16High_AsmOperand">;

def SImm16_AsmOperand : ImmAsmOperand<"SImm16">;
def simm16 : Immediate<i32, [{ return isInt<16>(Imm); }], "SImm16_AsmOperand">;

def UImm5_AsmOperand : ImmAsmOperand<"UImm5">;
def uimm5 : Immediate<i32, [{ return isUInt<5>(Imm); }], "UImm5_AsmOperand">;

//===----------------------------------------------------------------------===//
// Memory address operands
//===----------------------------------------------------------------------===//
/// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Xtensa/XtensaOperands.td#L139
def memri : Operand<iPTR> {
  let MIOperandInfo = (ops GPR:$ra, i16imm:$imm);
  let EncoderMethod = "getMemRegEncoding";
  let DecoderMethod = "decodeMemRegOperand";
  let OperandType = "OPERAND_MEMORY";
  let PrintMethod = "printMemOperand";
}

def addrimm : ComplexPattern<iPTR, 2, "selectAddrRegImm">;

//===----------------------------------------------------------------------===//
// Symbolic address operands
//===----------------------------------------------------------------------===//
def brtarget : Operand<OtherVT> {
  let PrintMethod = "printBranchTarget";
  let EncoderMethod = "getBranchTargetEncoding";
  let DecoderMethod = "decodeBranchOperand";
  //let ParserMatchClass = XtensaPCRelTargetAsmOperand; TODO
}

def  calltarget : Operand<iPTR> {
  let PrintMethod = "printCallOperand";
  let EncoderMethod = "getCallEncoding";
  let DecoderMethod = "decodeCallOperand";
  //let ParserMatchClass = XtensaPCRelTargetAsmOperand;
}