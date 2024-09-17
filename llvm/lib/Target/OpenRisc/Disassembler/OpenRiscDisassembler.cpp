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

#define DEBUG_TYPE "openrisc-disassembler"

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
private:
  DecodeStatus getInstruction32(MCInst &Instr, uint64_t &Size,
                                ArrayRef<uint8_t> Bytes, uint64_t Address,
                                raw_ostream &CStream) const;
};
} // end anonymous namespace

static MCDisassembler *createOpenRiscDisassembler(const Target &T,
                                                const MCSubtargetInfo &STI,
                                                MCContext &Ctx) {
  return new OpenRiscDisassembler(STI, Ctx, true);
}

static bool tryAddingSymbolicOperand(int64_t Value, bool isBranch,
                                     uint64_t Address, uint64_t Offset,
                                     uint64_t InstSize, MCInst &MI,
                                     const void *Decoder) {
  const MCDisassembler *Dis = static_cast<const MCDisassembler *>(Decoder);
  return Dis->tryAddingSymbolicOperand(MI, Value, Address, isBranch, Offset, /*OpSize=*/0,
                                       InstSize);
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

static DecodeStatus decodeSImm16(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isInt<16>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignExtend64<16>(Imm)));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUImm5(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<5>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeUImm16(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<16>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm16(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<16>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm32(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<32>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeImm16High(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<16>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(Imm));
  return MCDisassembler::Success;
}

static DecodeStatus decodeBranchOperand(MCInst &Inst, uint64_t Imm,
                                        int64_t Address, const void *Decoder) {
  assert(isUInt<26>(Imm) && "Invalid immediate");
  if (!tryAddingSymbolicOperand(SignExtend64<8>(Imm) + 4 + Address, true,
                              Address, 0, 3, Inst, Decoder))
  Inst.addOperand(MCOperand::createImm(SignExtend64<8>(Imm)));

  return MCDisassembler::Success;
}

static DecodeStatus decodeCallOperand(MCInst &Inst, uint64_t Imm,
                                      int64_t Address, const void *Decoder) {
  assert(isUInt<26>(Imm) && "Invalid immediate");
  Inst.addOperand(MCOperand::createImm(SignExtend64<26>(Imm << 2)));
  return MCDisassembler::Success;
}

#include "OpenRiscGenDisassemblerTables.inc"

DecodeStatus OpenRiscDisassembler::getInstruction32(MCInst &MI, uint64_t &Size,
                                                 ArrayRef<uint8_t> Bytes,
                                                 uint64_t Address,
                                                 raw_ostream &CS) const {
  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }
  Size = 4;

  uint32_t Insn =  support::endian::read32be(Bytes.data());
  return decodeInstruction(DecoderTable32, MI, Insn, Address, this, STI);
}

DecodeStatus OpenRiscDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                                ArrayRef<uint8_t> Bytes,
                                                uint64_t Address,
                                                raw_ostream &CS) const {
  return getInstruction32(MI, Size, Bytes, Address, CS);
}
