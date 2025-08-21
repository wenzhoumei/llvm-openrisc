//===-- OR1KMCObjectWriter.cpp - OR1K ELF writer --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/OR1KFixupKinds.h"
#include "MCTargetDesc/OR1KMCAsmInfo.h"
#include "MCTargetDesc/OR1KMCTargetDesc.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCValue.h"
#include "llvm/Support/ErrorHandling.h"

using namespace llvm;

namespace {
class OR1KObjectWriter : public MCELFObjectTargetWriter {
public:
  OR1KObjectWriter(uint8_t OSABI);

  virtual ~OR1KObjectWriter();

protected:
  unsigned getRelocType(const MCFixup &, const MCValue &,
                        bool IsPCRel) const override;
};
} // namespace

OR1KObjectWriter::OR1KObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_OPENRISC,
                              /*HasRelocationAddend=*/true) {}

OR1KObjectWriter::~OR1KObjectWriter() {}


unsigned OR1KObjectWriter::getRelocType(const MCFixup &Fixup,
                                        const MCValue &Target,
                                        bool IsPCRel) const {
  switch ((unsigned)Fixup.getKind()) {
  default:
    reportError(Fixup.getLoc(), "unsupported relocation type");
    return ELF::R_OR32_NONE;
  case FK_Data_4:
    return ELF::R_OR32_32;
  case OR1K::fixup_or1k_hi16:
    return ELF::R_OR32_CONSTH;
  case OR1K::fixup_or1k_lo16:
    return ELF::R_OR32_CONST;
  case OR1K::fixup_or1k_branch:
    return ELF::R_OR32_JUMPTARG;
  }
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createOR1KObjectWriter(uint8_t OSABI, bool IsLittleEndian) {
  return std::make_unique<OR1KObjectWriter>(OSABI);
}
