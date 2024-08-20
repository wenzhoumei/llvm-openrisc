//===-- OpenRiscTargetStreamer.cpp - OpenRisc Target Streamer Methods ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides OpenRisc specific target streamer methods.
//
//===----------------------------------------------------------------------===//

#include "OpenRiscTargetStreamer.h"
#include "OpenRiscInstPrinter.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCObjectFileInfo.h"
#include "llvm/MC/MCSectionELF.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/FormattedStream.h"

using namespace llvm;

static std::string getLiteralSectionName(StringRef CSectionName) {
  std::size_t Pos = CSectionName.find(".text");
  std::string SectionName;
  if (Pos != std::string::npos) {
    SectionName = CSectionName.substr(0, Pos);

    if (Pos > 0)
      SectionName += ".text";

    CSectionName = CSectionName.drop_front(Pos);
    CSectionName.consume_front(".text");

    SectionName += ".literal";
    SectionName += CSectionName;
  } else {
    SectionName = CSectionName;
    SectionName += ".literal";
  }
  return SectionName;
}

OpenRiscTargetStreamer::OpenRiscTargetStreamer(MCStreamer &S)
    : MCTargetStreamer(S) {}

OpenRiscTargetAsmStreamer::OpenRiscTargetAsmStreamer(MCStreamer &S,
                                                 formatted_raw_ostream &OS)
    : OpenRiscTargetStreamer(S), OS(OS) {}

void OpenRiscTargetAsmStreamer::emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                                          bool SwitchLiteralSection, SMLoc L) {
  SmallString<60> Str;
  raw_svector_ostream LiteralStr(Str);

  LiteralStr << "\t.literal " << LblSym->getName() << ", ";

  if (auto CE = dyn_cast<MCConstantExpr>(Value)) {
    LiteralStr << CE->getValue() << "\n";
  } else if (auto SRE = dyn_cast<MCSymbolRefExpr>(Value)) {
    const MCSymbol &Sym = SRE->getSymbol();
    LiteralStr << Sym.getName() << "\n";
  } else {
    llvm_unreachable("unexpected constant pool entry type");
  }

  OS << LiteralStr.str();
}

void OpenRiscTargetAsmStreamer::emitLiteralPosition() {
  OS << "\t.literal_position\n";
}

void OpenRiscTargetAsmStreamer::startLiteralSection(MCSection *BaseSection) {
  emitLiteralPosition();
}

OpenRiscTargetELFStreamer::OpenRiscTargetELFStreamer(MCStreamer &S)
    : OpenRiscTargetStreamer(S) {}

void OpenRiscTargetELFStreamer::emitLiteral(MCSymbol *LblSym, const MCExpr *Value,
                                          bool SwitchLiteralSection, SMLoc L) {
  MCStreamer &OutStreamer = getStreamer();
  if (SwitchLiteralSection) {
    MCContext &Context = OutStreamer.getContext();
    auto *CS = static_cast<MCSectionELF *>(OutStreamer.getCurrentSectionOnly());
    std::string SectionName = getLiteralSectionName(CS->getName());

    MCSection *ConstSection = Context.getELFSection(
        SectionName, ELF::SHT_PROGBITS, ELF::SHF_EXECINSTR | ELF::SHF_ALLOC);

    OutStreamer.pushSection();
    OutStreamer.switchSection(ConstSection);
  }

  OutStreamer.emitLabel(LblSym, L);
  OutStreamer.emitValue(Value, 4, L);

  if (SwitchLiteralSection) {
    OutStreamer.popSection();
  }
}

void OpenRiscTargetELFStreamer::startLiteralSection(MCSection *BaseSection) {
  MCContext &Context = getStreamer().getContext();

  std::string SectionName = getLiteralSectionName(BaseSection->getName());

  MCSection *ConstSection = Context.getELFSection(
      SectionName, ELF::SHT_PROGBITS, ELF::SHF_EXECINSTR | ELF::SHF_ALLOC);

  ConstSection->setAlignment(Align(4));
}

MCELFStreamer &OpenRiscTargetELFStreamer::getStreamer() {
  return static_cast<MCELFStreamer &>(Streamer);
}
