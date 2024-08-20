//===- OpenRiscAsmPrinter.cpp OpenRisc LLVM Assembly Printer ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains a printer that converts from our internal representation
// of machine-dependent LLVM code to GAS-format OpenRisc assembly language.
//
//===----------------------------------------------------------------------===//

#include "OpenRiscAsmPrinter.h"
#include "MCTargetDesc/OpenRiscMCExpr.h"
#include "MCTargetDesc/OpenRiscTargetStreamer.h"
#include "TargetInfo/OpenRiscTargetInfo.h"
#include "OpenRiscConstantPoolValue.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/CodeGen/MachineModuleInfoImpls.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInstBuilder.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbol.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

static MCSymbolRefExpr::VariantKind
getModifierVariantKind(OpenRiscCP::OpenRiscCPModifier Modifier) {
  switch (Modifier) {
  case OpenRiscCP::no_modifier:
    return MCSymbolRefExpr::VK_None;
  case OpenRiscCP::TPOFF:
    return MCSymbolRefExpr::VK_TPOFF;
  }
  report_fatal_error("Invalid OpenRiscCPModifier!");
}

void OpenRiscAsmPrinter::emitInstruction(const MachineInstr *MI) {
  unsigned Opc = MI->getOpcode();

  switch (Opc) {
  case OpenRisc::BR_JT:
    EmitToStreamer(
        *OutStreamer,
        MCInstBuilder(OpenRisc::JX).addReg(MI->getOperand(0).getReg()));
    return;
  default:
    MCInst LoweredMI;
    lowerToMCInst(MI, LoweredMI);
    EmitToStreamer(*OutStreamer, LoweredMI);
    return;
  }
}

void OpenRiscAsmPrinter::emitMachineConstantPoolValue(
    MachineConstantPoolValue *MCPV) {
  OpenRiscConstantPoolValue *ACPV = static_cast<OpenRiscConstantPoolValue *>(MCPV);
  MCSymbol *MCSym;

  if (ACPV->isBlockAddress()) {
    const BlockAddress *BA =
        cast<OpenRiscConstantPoolConstant>(ACPV)->getBlockAddress();
    MCSym = GetBlockAddressSymbol(BA);
  } else if (ACPV->isJumpTable()) {
    unsigned Idx = cast<OpenRiscConstantPoolJumpTable>(ACPV)->getIndex();
    MCSym = this->GetJTISymbol(Idx, false);
  } else {
    assert(ACPV->isExtSymbol() && "unrecognized constant pool value");
    OpenRiscConstantPoolSymbol *OpenRiscSym = cast<OpenRiscConstantPoolSymbol>(ACPV);
    const char *SymName = OpenRiscSym->getSymbol();

    if (OpenRiscSym->isPrivateLinkage()) {
      const DataLayout &DL = getDataLayout();
      MCSym = OutContext.getOrCreateSymbol(Twine(DL.getPrivateGlobalPrefix()) +
                                           SymName);
    } else {
      MCSym = OutContext.getOrCreateSymbol(SymName);
    }
  }

  MCSymbol *LblSym = GetCPISymbol(ACPV->getLabelId());
  auto *TS =
      static_cast<OpenRiscTargetStreamer *>(OutStreamer->getTargetStreamer());
  MCSymbolRefExpr::VariantKind VK = getModifierVariantKind(ACPV->getModifier());

  if (ACPV->getModifier() != OpenRiscCP::no_modifier) {
    std::string SymName(MCSym->getName());
    StringRef Modifier = ACPV->getModifierText();
    SymName += Modifier;
    MCSym = OutContext.getOrCreateSymbol(SymName);
  }

  const MCExpr *Expr = MCSymbolRefExpr::create(MCSym, VK, OutContext);
  TS->emitLiteral(LblSym, Expr, false);
}

void OpenRiscAsmPrinter::emitMachineConstantPoolEntry(
    const MachineConstantPoolEntry &CPE, int i) {
  if (CPE.isMachineConstantPoolEntry()) {
    OpenRiscConstantPoolValue *ACPV =
        static_cast<OpenRiscConstantPoolValue *>(CPE.Val.MachineCPVal);
    ACPV->setLabelId(i);
    emitMachineConstantPoolValue(CPE.Val.MachineCPVal);
  } else {
    MCSymbol *LblSym = GetCPISymbol(i);
    auto *TS =
        static_cast<OpenRiscTargetStreamer *>(OutStreamer->getTargetStreamer());
    const Constant *C = CPE.Val.ConstVal;
    const MCExpr *Value = nullptr;

    Type *Ty = C->getType();
    if (const auto *CFP = dyn_cast<ConstantFP>(C)) {
      Value = MCConstantExpr::create(
          CFP->getValueAPF().bitcastToAPInt().getSExtValue(), OutContext);
    } else if (const auto *CI = dyn_cast<ConstantInt>(C)) {
      Value = MCConstantExpr::create(CI->getValue().getSExtValue(), OutContext);
    } else if (isa<PointerType>(Ty)) {
      Value = lowerConstant(C);
    } else {
      llvm_unreachable("unexpected constant pool entry type");
    }

    TS->emitLiteral(LblSym, Value, false);
  }
}

// EmitConstantPool - Print to the current output stream assembly
// representations of the constants in the constant pool MCP. This is
// used to print out constants which have been "spilled to memory" by
// the code generator.
void OpenRiscAsmPrinter::emitConstantPool() {
  const Function &F = MF->getFunction();
  const MachineConstantPool *MCP = MF->getConstantPool();
  const std::vector<MachineConstantPoolEntry> &CP = MCP->getConstants();
  if (CP.empty())
    return;

  OutStreamer->pushSection();

  auto *TS =
      static_cast<OpenRiscTargetStreamer *>(OutStreamer->getTargetStreamer());
  MCSection *CS = getObjFileLowering().SectionForGlobal(&F, TM);
  TS->startLiteralSection(CS);

  int CPIdx = 0;
  for (const MachineConstantPoolEntry &CPE : CP) {
    emitMachineConstantPoolEntry(CPE, CPIdx++);
  }

  OutStreamer->popSection();
}

MCSymbol *
OpenRiscAsmPrinter::GetConstantPoolIndexSymbol(const MachineOperand &MO) const {
  // Create a symbol for the name.
  return GetCPISymbol(MO.getIndex());
}

MCSymbol *OpenRiscAsmPrinter::GetJumpTableSymbol(const MachineOperand &MO) const {
  return GetJTISymbol(MO.getIndex());
}

MCOperand
OpenRiscAsmPrinter::LowerSymbolOperand(const MachineOperand &MO,
                                     MachineOperand::MachineOperandType MOTy,
                                     unsigned Offset) const {
  const MCSymbol *Symbol;
  OpenRiscMCExpr::VariantKind Kind = OpenRiscMCExpr::VK_OpenRisc_None;

  switch (MOTy) {
  case MachineOperand::MO_GlobalAddress:
    Symbol = getSymbol(MO.getGlobal());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_MachineBasicBlock:
    Symbol = MO.getMBB()->getSymbol();
    break;
  case MachineOperand::MO_BlockAddress:
    Symbol = GetBlockAddressSymbol(MO.getBlockAddress());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_ExternalSymbol:
    Symbol = GetExternalSymbolSymbol(MO.getSymbolName());
    Offset += MO.getOffset();
    break;
  case MachineOperand::MO_JumpTableIndex:
    Symbol = GetJumpTableSymbol(MO);
    break;
  case MachineOperand::MO_ConstantPoolIndex:
    Symbol = GetConstantPoolIndexSymbol(MO);
    Offset += MO.getOffset();
    break;
  default:
    report_fatal_error("<unknown operand type>");
  }

  const MCExpr *ME =
      MCSymbolRefExpr::create(Symbol, MCSymbolRefExpr::VK_None, OutContext);
  ME = OpenRiscMCExpr::create(ME, Kind, OutContext);

  if (Offset) {
    // Assume offset is never negative.
    assert(Offset > 0);

    const MCConstantExpr *OffsetExpr =
        MCConstantExpr::create(Offset, OutContext);
    ME = MCBinaryExpr::createAdd(ME, OffsetExpr, OutContext);
  }

  return MCOperand::createExpr(ME);
}

MCOperand OpenRiscAsmPrinter::lowerOperand(const MachineOperand &MO,
                                         unsigned Offset) const {
  MachineOperand::MachineOperandType MOTy = MO.getType();

  switch (MOTy) {
  case MachineOperand::MO_Register:
    // Ignore all implicit register operands.
    if (MO.isImplicit())
      break;
    return MCOperand::createReg(MO.getReg());
  case MachineOperand::MO_Immediate:
    return MCOperand::createImm(MO.getImm() + Offset);
  case MachineOperand::MO_RegisterMask:
    break;
  case MachineOperand::MO_GlobalAddress:
  case MachineOperand::MO_MachineBasicBlock:
  case MachineOperand::MO_BlockAddress:
  case MachineOperand::MO_ExternalSymbol:
  case MachineOperand::MO_JumpTableIndex:
  case MachineOperand::MO_ConstantPoolIndex:
    return LowerSymbolOperand(MO, MOTy, Offset);
  default:
    report_fatal_error("unknown operand type");
  }

  return MCOperand();
}

void OpenRiscAsmPrinter::lowerToMCInst(const MachineInstr *MI,
                                     MCInst &OutMI) const {
  OutMI.setOpcode(MI->getOpcode());

  for (unsigned i = 0, e = MI->getNumOperands(); i != e; ++i) {
    const MachineOperand &MO = MI->getOperand(i);
    MCOperand MCOp = lowerOperand(MO);

    if (MCOp.isValid())
      OutMI.addOperand(MCOp);
  }
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOpenRiscAsmPrinter() {
  RegisterAsmPrinter<OpenRiscAsmPrinter> A(getTheOpenRiscTarget());
}
