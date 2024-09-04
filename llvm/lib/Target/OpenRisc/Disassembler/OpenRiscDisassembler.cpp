//===-- OpenRiscDisassembler.cpp - Disassembler for OpenRisc ------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the OpenRiscDisassembler class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/OpenRiscMCTargetDesc.h"
#include "TargetInfo/OpenRiscTargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Endian.h"

using namespace llvm;

#define DEBUG_TYPE "OpenRisc-disassembler"

using DecodeStatus = MCDisassembler::DecodeStatus;

namespace {

class OpenRiscDisassembler : public MCDisassembler {
  bool IsLittleEndian;

public:
  OpenRiscDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx, bool isLE)
      : MCDisassembler(STI, Ctx), IsLittleEndian(isLE) {}

  DecodeStatus getInstruction(MCInst &Instr, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};
} // end anonymous namespace

static MCDisassembler *createOpenRiscDisassembler(const Target &T,
                                                const MCSubtargetInfo &STI,
                                                MCContext &Ctx) {
  return new OpenRiscDisassembler(STI, Ctx, true);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOpenRiscDisassembler() {
  TargetRegistry::RegisterMCDisassembler(getTheOpenRiscTarget(),
                                         createOpenRiscDisassembler);
}

static const unsigned GPRDecoderTable[] = {
    OpenRisc::R0,  OpenRisc::R1,  OpenRisc::R2,  OpenRisc::R3,  OpenRisc::R4,  OpenRisc::R5,
    OpenRisc::R6,  OpenRisc::R7,  OpenRisc::R8,  OpenRisc::R9,  OpenRisc::R10, OpenRisc::R11,
    OpenRisc::R12, OpenRisc::R13, OpenRisc::R14, OpenRisc::R15, OpenRisc::R16, OpenRisc::R17,
    OpenRisc::R18, OpenRisc::R19, OpenRisc::R20, OpenRisc::R21, OpenRisc::R22, OpenRisc::R23,
    OpenRisc::R24, OpenRisc::R25, OpenRisc::R26, OpenRisc::R27, OpenRisc::R28, OpenRisc::R29,
    OpenRisc::R30, OpenRisc::R31
};

static DecodeStatus DecodeGPRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                          uint64_t Address,
                                          const void *Decoder) {
  if (RegNo >= std::size(GPRDecoderTable))
    return MCDisassembler::Fail;

  unsigned Reg = GPRDecoderTable[RegNo];
  Inst.addOperand(MCOperand::createReg(Reg));
  return MCDisassembler::Success;
}
static DecodeStatus decodeMemRegOperand(MCInst &Inst, uint64_t Imm,
                                       int64_t Address, const void *Decoder) {
  assert(isUInt<12>(Imm) && "Invalid immediate");
  DecodeGPRRegisterClass(Inst, Imm & 0xf, Address, Decoder);
  Inst.addOperand(MCOperand::createImm((Imm >> 2) & 0x3fc));
  return MCDisassembler::Success;
}

/// Read three bytes from the ArrayRef and return 24 bit data
static DecodeStatus readInstruction24(ArrayRef<uint8_t> Bytes, uint64_t Address,
                                      uint64_t &Size, uint32_t &Insn,
                                      bool IsLittleEndian) {
  // We want to read exactly 3 Bytes of data.
  if (Bytes.size() < 3) {
    Size = 0;
    return MCDisassembler::Fail;
  }

  if (!IsLittleEndian) {
    report_fatal_error("Big-endian mode currently is not supported!");
  } else {
    Insn = (Bytes[2] << 16) | (Bytes[1] << 8) | (Bytes[0] << 0);
  }

  Size = 3;
  return MCDisassembler::Success;
}

#include "OpenRiscGenDisassemblerTables.inc"

DecodeStatus OpenRiscDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                                ArrayRef<uint8_t> Bytes,
                                                uint64_t Address,
                                                raw_ostream &CS) const {
  uint32_t Insn;
  DecodeStatus Result;

  Result = readInstruction24(Bytes, Address, Size, Insn, IsLittleEndian);
  if (Result == MCDisassembler::Fail)
    return MCDisassembler::Fail;
  LLVM_DEBUG(dbgs() << "Trying OpenRisc 24-bit instruction table :\n");
  Result = decodeInstruction(DecoderTable24, MI, Insn, Address, this, STI);
  return Result;
}
