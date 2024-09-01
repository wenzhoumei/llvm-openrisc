//===- OpenRiscOperands.td - OpenRisc instruction operands -------*- tblgen-*--===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
/// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Xtensa/XtensaOperands.td#L18
class Immediate<ValueType vt, code pred, string asmOp>
  : Operand<vt>, ImmLeaf<vt, pred> {
    let PrintMethod = "print"#asmOp;
    let ParserMatchClass = !cast<AsmOperandClass>(asmOp);
}

// Immediate operations
def uimm16hi : Immediate<i32, [{ return Imm & 0xFFFF == 0); }], "Imm16High">;
def uimm5 : Immediate<i32, [{ return isUInt<5>(Imm); }], "Imm5">;

//===----------------------------------------------------------------------===//
// Memory address operands
//===----------------------------------------------------------------------===//
/// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Xtensa/XtensaOperands.td#L139
class mem<Operand offset> : Operand<i32> {
  let MIOperandInfo = (ops GPR, offset);
  let EncoderMethod = "getMemRegEncoding";
  let OperandType = "OPERAND_MEMORY";
  let PrintMethod = "printMemOperand";
}

def mem8   : mem<offset8m8> {
  let DecoderMethod = "decodeMem8Operand";
}

def mem16  : mem<offset8m16> {
  let DecoderMethod = "decodeMem16Operand";
}

def mem32  : mem<offset8m32> {
  let DecoderMethod = "decodeMem32Operand";
}

def mem32n : mem<offset4m32> {
  let DecoderMethod = "decodeMem32nOperand";
}

def addrimm : ComplexPattern<iPTR, 2, "selectMemRegAddr", [frameindex]>; /// https://vscode.dev/github/wenzhoumei/llvm-openrisc/blob/main/llvm/lib/Target/Mips/MipsInstrInfo.td#L1305