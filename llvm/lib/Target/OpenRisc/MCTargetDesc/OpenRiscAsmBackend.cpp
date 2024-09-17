//===-- OpenRiscMCAsmBackend.cpp - OpenRisc assembler backend -----------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/OpenRiscFixupKinds.h"
#include "MCTargetDesc/OpenRiscMCTargetDesc.h"
#include "llvm/MC/MCAsmBackend.h"
#include "llvm/MC/MCAssembler.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCELFObjectWriter.h"
#include "llvm/MC/MCFixupKindInfo.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCObjectWriter.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace llvm {
class MCObjectTargetWriter;
class OpenRiscMCAsmBackend : public MCAsmBackend {
  uint8_t OSABI;
  bool isBigEndian;

public:
  OpenRiscMCAsmBackend(uint8_t osABI, bool isLE)
      : MCAsmBackend(llvm::endianness::big), OSABI(osABI),
        isBigEndian(isLE) {}

  unsigned getNumFixupKinds() const override {
    return OpenRisc::NumTargetFixupKinds;
  }
  const MCFixupKindInfo &getFixupKindInfo(MCFixupKind Kind) const override;
  void applyFixup(const MCAssembler &Asm, const MCFixup &Fixup,
                  const MCValue &Target, MutableArrayRef<char> Data,
                  uint64_t Value, bool IsResolved,
                  const MCSubtargetInfo *STI) const override;
  bool mayNeedRelaxation(const MCInst &Inst,
                         const MCSubtargetInfo &STI) const override;
  void relaxInstruction(MCInst &Inst,
                        const MCSubtargetInfo &STI) const override;
  bool writeNopData(raw_ostream &OS, uint64_t Count,
                    const MCSubtargetInfo *STI) const override;

  std::unique_ptr<MCObjectTargetWriter> createObjectTargetWriter() const override {
    return createOpenRiscObjectWriter(OSABI, isBigEndian);
  }
};
} // namespace llvm

const MCFixupKindInfo &
OpenRiscMCAsmBackend::getFixupKindInfo(MCFixupKind Kind) const {
  const static MCFixupKindInfo Infos[OpenRisc::NumTargetFixupKinds] = {
      // name                     offset bits  flags
      {"fixup_openrisc_branch_6", 0, 16, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_openrisc_branch_8", 16, 8, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_openrisc_branch_12", 12, 12, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_openrisc_jump_18", 6, 18, MCFixupKindInfo::FKF_IsPCRel},
      {"fixup_openrisc_call_18", 6, 18,
       MCFixupKindInfo::FKF_IsPCRel |
           MCFixupKindInfo::FKF_IsAlignedDownTo32Bits},
      {"fixup_openrisc_l32r_16", 8, 16,
       MCFixupKindInfo::FKF_IsPCRel |
           MCFixupKindInfo::FKF_IsAlignedDownTo32Bits}};

  if (Kind < FirstTargetFixupKind)
    return MCAsmBackend::getFixupKindInfo(Kind);
  assert(unsigned(Kind - FirstTargetFixupKind) < getNumFixupKinds() &&
         "Invalid kind!");
  return Infos[Kind - FirstTargetFixupKind];
}

static uint64_t adjustFixupValue(const MCFixup &Fixup, uint64_t Value,
                                 MCContext &Ctx) {
  unsigned Kind = Fixup.getKind();
  switch (Kind) {
  default:
    llvm_unreachable("Unknown fixup kind!");
  case FK_Data_1:
  case FK_Data_2:
  case FK_Data_4:
  case FK_Data_8:
    return Value;
  case OpenRisc::fixup_openrisc_branch_6: {
    Value -= 4;
    if (!isInt<6>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    unsigned Hi2 = (Value >> 4) & 0x3;
    unsigned Lo4 = Value & 0xf;
    return (Hi2 << 4) | (Lo4 << 12);
  }
  case OpenRisc::fixup_openrisc_branch_8:
    Value -= 4;
    if (!isInt<8>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0xff);
  case OpenRisc::fixup_openrisc_branch_12:
    Value -= 4;
    if (!isInt<12>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0xfff);
  case OpenRisc::fixup_openrisc_jump_18:
    Value -= 4;
    if (!isInt<18>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    return (Value & 0x3ffff);
  case OpenRisc::fixup_openrisc_call_18:
    Value -= 4;
    if (!isInt<20>(Value))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup value must be 4-byte aligned");
    return (Value & 0xffffc) >> 2;
  case OpenRisc::fixup_openrisc_l32r_16:
    unsigned Offset = Fixup.getOffset();
    if (Offset & 0x3)
      Value -= 4;
    if (!isInt<18>(Value) && (Value & 0x20000))
      Ctx.reportError(Fixup.getLoc(), "fixup value out of range");
    if (Value & 0x3)
      Ctx.reportError(Fixup.getLoc(), "fixup value must be 4-byte aligned");
    return (Value & 0x3fffc) >> 2;
  }
}

static unsigned getSize(unsigned Kind) {
  switch (Kind) {
  default:
    return 3;
  case MCFixupKind::FK_Data_4:
    return 4;
  case OpenRisc::fixup_openrisc_branch_6:
    return 2;
  }
}

void OpenRiscMCAsmBackend::applyFixup(const MCAssembler &Asm,
                                    const MCFixup &Fixup, const MCValue &Target,
                                    MutableArrayRef<char> Data, uint64_t Value,
                                    bool IsResolved,
                                    const MCSubtargetInfo *STI) const {
  MCContext &Ctx = Asm.getContext();
  MCFixupKindInfo Info = getFixupKindInfo(Fixup.getKind());

  Value = adjustFixupValue(Fixup, Value, Ctx);

  // Shift the value into position.
  Value <<= Info.TargetOffset;

  if (!Value)
    return; // Doesn't change encoding.

  unsigned Offset = Fixup.getOffset();
  unsigned FullSize = getSize(Fixup.getKind());

  for (unsigned i = 0; i != FullSize; ++i) {
    Data[Offset + i] |= uint8_t((Value >> (i * 8)) & 0xff);
  }
}

bool OpenRiscMCAsmBackend::mayNeedRelaxation(const MCInst &Inst,
                                           const MCSubtargetInfo &STI) const {
  return false;
}

void OpenRiscMCAsmBackend::relaxInstruction(MCInst &Inst,
                                          const MCSubtargetInfo &STI) const {}

bool OpenRiscMCAsmBackend::writeNopData(raw_ostream &OS, uint64_t Count,
                                      const MCSubtargetInfo *STI) const {
  uint64_t NumNops = Count / 4;

  for (uint64_t i = 0; i != NumNops; ++i) {
    // Currently just big-endian machine supported,
    // but probably big-endian will be also implemented in future
    if (isBigEndian) {
      OS.write("\x15", 1);
      OS.write("\x00", 1);
      OS.write("\x00", 1);
      OS.write("\x00", 1);
    } else {
      OS.write("\x00", 1);
      OS.write("\x00", 1);
      OS.write("\x00", 1);
      OS.write("\x15", 1);
    }
    Count -= 4;
  }

  switch (Count) {
  default:
    break;
  case 1:
  case 2:
  case 3:
    llvm_unreachable("NOP data length should be a multiple of 4 bytes");
  }

  return true;
}

MCAsmBackend *llvm::createOpenRiscMCAsmBackend(const Target &T,
                                             const MCSubtargetInfo &STI,
                                             const MCRegisterInfo &MRI,
                                             const MCTargetOptions &Options) {
  uint8_t OSABI =
      MCELFObjectTargetWriter::getOSABI(STI.getTargetTriple().getOS());
  return new llvm::OpenRiscMCAsmBackend(OSABI, true);
}
