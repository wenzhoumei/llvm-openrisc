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
def Imm16High_AsmOperand : ImmAsmOperand<"Imm8">;
def uimm16hi : Immediate<i32, [{ return Imm & 0xFFFF == 0; }], "Imm16High_AsmOperand">;

def Imm5_AsmOperand : ImmAsmOperand<"Imm5">;
def uimm5 : Immediate<i32, [{ return isUInt<5>(Imm); }], "Imm5_AsmOperand">;

//===----------------------------------------------------------------------===//
// Memory address operands
//===----------------------------------------------------------------------===//
/// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Xtensa/XtensaOperands.td#L139
def memri : Operand<i32> {
  let MIOperandInfo = (ops GPR:$ra, i16imm:$imm);
  let EncoderMethod = "getMemRegEncoding";
  let OperandType = "OPERAND_MEMORY";
  let PrintMethod = "printMemOperand";
}

def addrimm : ComplexPattern<iPTR, 2, "selectMemRegAddr", [frameindex]>; /// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Mips/MipsInstrInfo.td#L1305