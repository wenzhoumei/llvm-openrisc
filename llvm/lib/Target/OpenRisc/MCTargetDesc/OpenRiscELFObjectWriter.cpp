//===-- OpenRiscMCObjectWriter.cpp - OpenRisc ELF writer ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/OpenRiscMCTargetDesc.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/BinaryFormat/ELF.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"
#include <cassert>
#include <cstdint>

using namespace llvm;

namespace {
class OpenRiscObjectWriter : public MCELFObjectTargetWriter {
public:
  OpenRiscObjectWriter(uint8_t OSABI);

  virtual ~OpenRiscObjectWriter();

protected:
  unsigned getRelocType(MCContext &Ctx, const MCValue &Target,
                        const MCFixup &Fixup, bool IsPCRel) const override;
  bool needsRelocateWithSymbol(const MCValue &Val, const MCSymbol &Sym,
                               unsigned Type) const override;
};
} // namespace

OpenRiscObjectWriter::OpenRiscObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_OPENRISC,
                              /*HasRelocationAddend=*/true) {}

OpenRiscObjectWriter::~OpenRiscObjectWriter() {}

unsigned OpenRiscObjectWriter::getRelocType(MCContext &Ctx, const MCValue &Target,
                                          const MCFixup &Fixup,
                                          bool IsPCRel) const {

  switch ((unsigned)Fixup.getKind()) {
  case FK_Data_4:
    return ELF::R_OPENRISC_32;
  default:
    return ELF::R_OPENRISC_SLOT0_OP;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createOpenRiscObjectWriter(uint8_t OSABI, bool IsLittleEndian) {
  return std::make_unique<OpenRiscObjectWriter>(OSABI);
}

bool OpenRiscObjectWriter::needsRelocateWithSymbol(const MCValue &,
                                                 const MCSymbol &,
                                                 unsigned Type) const {
  return false;
}
