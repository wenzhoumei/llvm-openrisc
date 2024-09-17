//===- OpenRiscInstPrinter.cpp - Convert OpenRisc MCInst to asm syntax --------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This class prints an OpenRisc MCInst to a .s file.
//
//===----------------------------------------------------------------------===//

#include "OpenRiscInstPrinter.h"
#include "llvm/CodeGen/MachineOperand.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegister.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

#define DEBUG_TYPE "asm-printer"

#include "OpenRiscGenAsmWriter.inc"

static void printExpr(const MCExpr *Expr, raw_ostream &OS) {
  int Offset = 0;
  const MCSymbolRefExpr *SRE;

  if (!(SRE = cast<MCSymbolRefExpr>(Expr)))
    assert(false && "Unexpected MCExpr type.");

  MCSymbolRefExpr::VariantKind Kind = SRE->getKind();

  switch (Kind) {
  case MCSymbolRefExpr::VK_None:
    break;
  // TODO
  default:
    report_fatal_error("Invalid kind!");
  }

  OS << SRE->getSymbol();

  if (Offset) {
    if (Offset > 0)
      OS << '+';
    OS << Offset;
  }

  if (Kind != MCSymbolRefExpr::VK_None)
    OS << ')';
}

void OpenRiscInstPrinter::printOperand(const MCOperand &MC, raw_ostream &O) {
  if (MC.isReg())
    O << getRegisterName(MC.getReg());
  else if (MC.isImm())
    O << MC.getImm();
  else if (MC.isExpr())
    printExpr(MC.getExpr(), O);
  else
    report_fatal_error("Invalid operand");
}

void OpenRiscInstPrinter::printInst(const MCInst *MI, uint64_t Address,
                                  StringRef Annot, const MCSubtargetInfo &STI,
                                  raw_ostream &O) {
  printInstruction(MI, Address, O);
  printAnnotation(O, Annot);
}

void OpenRiscInstPrinter::printRegName(raw_ostream &O, MCRegister Reg) const {
  O << getRegisterName(Reg);
}

void OpenRiscInstPrinter::printOperand(const MCInst *MI, int OpNum,
                                     raw_ostream &O) {
  printOperand(MI->getOperand(OpNum), O);
}

void OpenRiscInstPrinter::printBranchTarget(const MCInst *MI, int OpNum,
                                          raw_ostream &OS) {
  const MCOperand &MC = MI->getOperand(OpNum);
  if (MI->getOperand(OpNum).isImm()) {
    int64_t Val = MC.getImm() + 4;
    OS << ". ";
    if (Val > 0)
      OS << '+';
    OS << Val;
  } else if (MC.isExpr())
    MC.getExpr()->print(OS, &MAI, true);
  else
    llvm_unreachable("Invalid operand");
}

void OpenRiscInstPrinter::printCallOperand(const MCInst *MI, int OpNum,
                                         raw_ostream &OS) {
  const MCOperand &MC = MI->getOperand(OpNum);
  if (MC.isImm()) {
    int64_t Val = MC.getImm() + 4;
    OS << ". ";
    if (Val > 0)
      OS << '+';
    OS << Val;
  } else if (MC.isExpr())
    MC.getExpr()->print(OS, &MAI, true);
  else
    llvm_unreachable("Invalid operand");
}

void OpenRiscInstPrinter::printImm16High(const MCInst *MI, int OpNum,
                                                   raw_ostream &O) {
  const MCOperand &MC = MI->getOperand(OpNum);

  if (MC.isImm()) {
    int64_t Value = MC.getImm();
    assert(((Value & 0xFFFF) == 0) && "Lower 16-bit not zero");
    O << "hi(" << Value << ")";
  } else if (MC.isExpr()) {
    O << "hi(";
    MC.getExpr()->print(O, &MAI, true);
    O << ")";
  } else
    printOperand(MI, OpNum, O);
}

void OpenRiscInstPrinter::printSImm16(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  const MCOperand &MC = MI->getOperand(OpNum);

  if (MC.isImm()) {
    int64_t Value = MC.getImm();
    assert((Value >= -32768 && Value <= 32767) && "Invalid argument");
    O << Value;
  } else if (MC.isExpr()) {
    O << "hi(";
    MC.getExpr()->print(O, &MAI, true);
    O << ")";
  } else
    printOperand(MI, OpNum, O);
}

void OpenRiscInstPrinter::printImm32(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  const MCOperand &MC = MI->getOperand(OpNum);

  if (MC.isImm()) {
    int64_t Value = MC.getImm();
    assert((Value >= -2147483648 && Value <= 4294967295) && "Invalid argument");
    O << Value;
  } else if (MC.isExpr()) {
    MC.getExpr()->print(O, &MAI, true);
  } else
    printOperand(MI, OpNum, O);
}

void OpenRiscInstPrinter::printImm16(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  const MCOperand &MC = MI->getOperand(OpNum);

  if (MC.isImm()) {
    int64_t Value = MC.getImm();
    assert((Value >= -32768 && Value <= 65535) && "Invalid argument");
    O << Value;
  } else if (MC.isExpr()) {
    O << "lo(";
    MC.getExpr()->print(O, &MAI, true);
    O << ")";
  } else
    printOperand(MI, OpNum, O);
}

void OpenRiscInstPrinter::printUImm16(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  const MCOperand &MC = MI->getOperand(OpNum);

  if (MC.isImm()) {
    int64_t Value = MC.getImm();
    assert((Value >= 0 && Value <= 65535) && "Invalid argument");
    O << Value;
  } else if (MC.isExpr()) {
    O << "lo(";
    MC.getExpr()->print(O, &MAI, true);
    O << ")";
  } else
    printOperand(MI, OpNum, O);
}

void OpenRiscInstPrinter::printUImm5(const MCInst *MI, int OpNum,
                                              raw_ostream &O) {
  const MCOperand &MC = MI->getOperand(OpNum);

  if (MC.isImm()) {
    int64_t Value = MC.getImm();
    assert((Value >= 0 && Value <= 31) && "Invalid argument");
    O << Value;
  } else
    printOperand(MI, OpNum, O);
}