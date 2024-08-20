//===-- OpenRiscMCExpr.cpp - OpenRisc specific MC expression classes ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the implementation of the assembly expression modifiers
// accepted by the OpenRisc architecture
//
//===----------------------------------------------------------------------===//

#include "OpenRiscMCExpr.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSymbolELF.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Object/ELF.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

#define DEBUG_TYPE "openriscmcexpr"

const OpenRiscMCExpr *OpenRiscMCExpr::create(const MCExpr *Expr, VariantKind Kind,
                                         MCContext &Ctx) {
  return new (Ctx) OpenRiscMCExpr(Expr, Kind);
}

void OpenRiscMCExpr::printImpl(raw_ostream &OS, const MCAsmInfo *MAI) const {
  bool HasVariant = getKind() != VK_OpenRisc_None;
  if (HasVariant)
    OS << '%' << getVariantKindName(getKind()) << '(';
  Expr->print(OS, MAI);
  if (HasVariant)
    OS << ')';
}

bool OpenRiscMCExpr::evaluateAsRelocatableImpl(MCValue &Res,
                                             const MCAssembler *Asm,
                                             const MCFixup *Fixup) const {
  return getSubExpr()->evaluateAsRelocatable(Res, Asm, Fixup);
}

void OpenRiscMCExpr::visitUsedExpr(MCStreamer &Streamer) const {
  Streamer.visitUsedExpr(*getSubExpr());
}

OpenRiscMCExpr::VariantKind OpenRiscMCExpr::getVariantKindForName(StringRef name) {
  return StringSwitch<OpenRiscMCExpr::VariantKind>(name).Default(
      VK_OpenRisc_Invalid);
}

StringRef OpenRiscMCExpr::getVariantKindName(VariantKind Kind) {
  switch (Kind) {
  default:
    llvm_unreachable("Invalid ELF symbol kind");
  }
}
