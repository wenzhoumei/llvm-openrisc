//===- OpenRiscOperands.td - OpenRisc instruction operands -------*- tblgen-*--===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

i16immsext8  : ImmLeaf<i16, [{ return isInt<8>(Imm); }]>;

// reference: /home/wenzhou/Dev/llvm-openrisc/llvm/lib/Target/Xtensa/XtensaOperands.td
class Immediate<ValueType vt, code pred, string asmOp>
  : Operand<vt>, ImmLeaf<vt, pred> {
    let PrintMethod = "print"#asmOp;
    let ParserMatchClass = !cast<AsmOperandClass>(asmOp);

}

def uimm16hi : Immediate<i32, [{ return Imm & 0xFFFF == 0); }], "Imm16High">;