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
#include "MCTargetDesc/OR1KFixupKinds.h"
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

  MCFixupKindInfo getFixupKindInfo(MCFixupKind Kind) const override;

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

MCFixupKindInfo OR1KAsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[] = {
    // name                    offset  bits flags
    {"fixup_or1k_branch",      0,      26,  0},
    {"fixup_or1k_hi16",        0,      16,  0},
    {"fixup_or1k_lo16",        0,      16,  0},
  };

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);

  assert(unsigned(Kind - FirstTargetFixupKind) < OR1K::NumTargetFixupKinds &&
         "Invalid kind!");

  return Infos[Kind - FirstTargetFixupKind];
}

static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext &Ctx) {
  unsigned Kind = Fixup.getKind();

  switch (Kind) {
  default:
    llvm_unreachable("Unknown OR1K fixup kind!");

  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
    return Value;

  case OR1K::fixup_or1k_branch:
      return (Value >> 2) & 0x03ffffff;

  case OR1K::fixup_or1k_hi16:
    return (Value >> 16) & 0xffff;

  case OR1K::fixup_or1k_lo16:
    return Value & 0xffff;
  }
}

void OR1KAsmBackend::applyFixup(const MCFragment &F, const MCFixup &Fixup,
                                const MCValue &Target,
                                MutableArrayRef<char> Data, uint64_t Value,
                                bool IsResolved) {
    maybeAddReloc(F, Fixup, Target, Value, IsResolved);
    MCContext &Ctx = getContext();
    MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

    Value = adjustFixupValue(Fixup, Value, Ctx);

    // Shift the value into the correct bit position in the instruction.
    Value <<= Info.TargetOffset;

    unsigned Offset = Fixup.getOffset();

    // Apply mask to each byte of instruction.
    for (unsigned i = 0; i != 4; ++i) {
      Data[Offset + 3 - i] |= uint8_t((Value >> (i * 8)) & 0xff);
    }
}

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
  return new llvm::OR1KAsmBackend(OSABI, false);
}
