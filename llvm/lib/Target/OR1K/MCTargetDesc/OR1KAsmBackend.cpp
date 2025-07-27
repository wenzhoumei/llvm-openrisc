//===-- OR1KAsmBackend.cpp - OR1K assembler backend ---------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/OR1KMCTargetDesc.h"
#include "llvm/ADT/bit.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/MC/MCFixup.h"
#include "llvm/Support/EndianStream.h"

using namespace llvm;

namespace llvm {
class MCObjectTargetWriter;
class OR1KAsmBackend : public MCAsmBackend {
public:
  OR1KAsmBackend(uint8_t OSABI, bool IsLittleEndian)
      : MCAsmBackend(llvm::endianness::big), OSABI(OSABI),
        IsLittleEndian(IsLittleEndian) {}

  void applyFixup(const MCFragment &, const MCFixup &, const MCValue &Target,
                  MutableArrayRef<char> Data, uint64_t Value,
                  bool IsResolved) override;

  bool mayNeedRelaxation(unsigned Opcode, ArrayRef<MCOperand> Operands,
                         const MCSubtargetInfo &STI) const override {
    return false;
  }

  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter>
  createObjectTargetWriter() const override {
    return createOR1KObjectWriter(OSABI, IsLittleEndian);
  }

private:
  uint8_t OSABI;
  bool IsLittleEndian;
};
} // namespace llvm

void OR1KAsmBackend::applyFixup(const MCFragment &F, const MCFixup &Fixup,
                                  const MCValue &Target,
                                  MutableArrayRef<char> Data, uint64_t Value,
                                  bool IsResolved) {}

bool OR1KAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                    const MCSubtargetInfo *STI) const {
  assert((Count % 4) == 0 && "NOP padding must be a multiple of 4 bytes");

  const uint32_t NopEncoding = 0x15000000;
  uint64_t NumNops = Count / 4;

  for (uint64_t i = 0; i != NumNops; ++i)
    support::endian::write(OS, NopEncoding, Endian);

  return true;
}

MCAsmBackend *llvm::createOR1KAsmBackend(const Target &T,
                                             const MCSubtargetInfo &STI,
                                             const MCRegisterInfo &MRI,
                                             const MCTargetOptions &Options) {
  uint8_t OSABI =
      MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new llvm::OR1KAsmBackend(OSABI, true);
}
