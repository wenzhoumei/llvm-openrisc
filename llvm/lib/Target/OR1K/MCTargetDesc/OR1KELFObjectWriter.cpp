//===-- OR1KMCObjectWriter.cpp - OR1K ELF writer --------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/OR1KMCTargetDesc.h"
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
class OR1KObjectWriter : public MCELFObjectTargetWriter {
public:
  OR1KObjectWriter(uint8_t OSABI);

  virtual ~OR1KObjectWriter();

protected:
  unsigned getRelocType(const MCFixup &, const MCValue &,
                        bool IsPCRel) const override;
  bool needsRelocateWithSymbol(const MCValue &, unsigned Type) const override;
};
} // namespace

OR1KObjectWriter::OR1KObjectWriter(uint8_t OSABI)
    : MCELFObjectTargetWriter(false, OSABI, ELF::EM_OPENRISC,
                              /*HasRelocationAddend=*/true) {}

OR1KObjectWriter::~OR1KObjectWriter() {}

unsigned OR1KObjectWriter::getRelocType(const MCFixup &Fixup,
                                          const MCValue &Target,
                                          bool IsPCRel) const {
  report_fatal_error("invalid fixup kind!");
}

std::unique_ptr<MCObjectTargetWriter>
llvm::createOR1KObjectWriter(uint8_t OSABI, bool IsLittleEndian) {
  return std::make_unique<OR1KObjectWriter>(OSABI);
}

bool OR1KObjectWriter::needsRelocateWithSymbol(const MCValue &,
                                                 unsigned Type) const {
  return false;
}
