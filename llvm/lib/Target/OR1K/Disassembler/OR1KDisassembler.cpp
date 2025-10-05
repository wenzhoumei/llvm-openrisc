//===-- OR1KDisassembler.cpp - Disassembler for OR1K ----------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the OR1KDisassembler class.
//
//===----------------------------------------------------------------------===//

#include "MCTargetDesc/OR1KMCTargetDesc.h"
#include "TargetInfo/OR1KTargetInfo.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCDecoderOps.h"
#include "llvm/MC/MCDisassembler/MCDisassembler.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/Support/Endian.h"
#include "llvm/MC/TargetRegistry.h"

using namespace llvm;

#define DEBUG_TYPE "or1k-disassembler"

using DecodeStatus = MCDisassembler::DecodeStatus;

namespace {

class OR1KDisassembler : public MCDisassembler {
  bool IsLittleEndian;

public:
  OR1KDisassembler(const MCSubtargetInfo &STI, MCContext &Ctx, bool IsLE)
      : MCDisassembler(STI, Ctx), IsLittleEndian(IsLE) {}

  DecodeStatus getInstruction(MCInst &MI, uint64_t &Size,
                              ArrayRef<uint8_t> Bytes, uint64_t Address,
                              raw_ostream &CStream) const override;
};

} // end anonymous namespace

static MCDisassembler *createOR1KDisassembler(const Target &,
                                              const MCSubtargetInfo &STI,
                                              MCContext &Ctx) {
  bool IsLE = Ctx.getTargetTriple().isLittleEndian();
  return new OR1KDisassembler(STI, Ctx, IsLE);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOR1KDisassembler() {
  TargetRegistry::RegisterMCDisassembler(getTheOR1KTarget(),
                                         createOR1KDisassembler);
}

static DecodeStatus DecodeGPRRegisterClass(MCInst &Inst, uint64_t RegNo,
                                           uint64_t /*Address*/,
                                           const void * /*Decoder*/) {
  if (RegNo > 31)
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createReg(OR1K::R0 + (unsigned)RegNo));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeUImmOperand(MCInst &Inst, uint64_t Imm,
                                      uint64_t /*Address*/,
                                      const void * /*Decoder*/) {
  if (!isUInt<N>(Imm))
    return MCDisassembler::Fail;
  Inst.addOperand(MCOperand::createImm((int64_t)Imm));
  return MCDisassembler::Success;
}

template <unsigned N>
static DecodeStatus decodeSImmOperand(MCInst &Inst, uint64_t Imm,
                                      uint64_t /*Address*/,
                                      const void * /*Decoder*/) {
  if (!isUInt<N>(Imm))
    return MCDisassembler::Fail;
  int64_t SImm = SignExtend64<N>(Imm);
  Inst.addOperand(MCOperand::createImm(SImm));
  return MCDisassembler::Success;
}

static bool tryAddingSymbolicOperand(int64_t Value, bool isBranch,
                                     uint64_t Address, uint64_t Offset,
                                     uint64_t InstSize, MCInst &MI,
                                     const void *Decoder) {
  const MCDisassembler *Dis = static_cast<const MCDisassembler *>(Decoder);
  return Dis->tryAddingSymbolicOperand(MI, Value, Address, isBranch, Offset,
                                       /*OpSize=*/0, InstSize);
}

static DecodeStatus decodePCRelOperand(MCInst &Inst, uint64_t Imm,
                                        uint64_t Address, const void *Decoder) {
  assert(isUInt<26>(Imm) && "Invalid OR1K branch/jump immediate");
  int64_t Off = SignExtend64<26>(Imm) << 2;

  if (!tryAddingSymbolicOperand(Address + Off, /*isBranch=*/true,
                                Address, /*Offset=*/0, /*InstSize=*/4,
                                Inst, Decoder))
    Inst.addOperand(MCOperand::createImm(Off));
  return MCDisassembler::Success;
}

#include "OR1KGenDisassemblerTables.inc"

static DecodeStatus readInstruction(ArrayRef<uint8_t> Bytes, uint64_t &Size,
                                      uint32_t &Insn, bool IsLittleEndian) {
  if (Bytes.size() < 4) {
    Size = 0;
    return MCDisassembler::Fail;
  }
  if (IsLittleEndian) {
    Insn = (uint32_t)Bytes[0]
         | (uint32_t)Bytes[1] << 8
         | (uint32_t)Bytes[2] << 16
         | (uint32_t)Bytes[3] << 24;
  } else {
    Insn = (uint32_t)Bytes[3]
         | (uint32_t)Bytes[2] << 8
         | (uint32_t)Bytes[1] << 16
         | (uint32_t)Bytes[0] << 24;
  }
  Size = 4;
  return MCDisassembler::Success;
}

DecodeStatus OR1KDisassembler::getInstruction(MCInst &MI, uint64_t &Size,
                                              ArrayRef<uint8_t> Bytes,
                                              uint64_t Address,
                                              raw_ostream &OS) const {
  uint32_t Insn;
  if (readInstruction(Bytes, Size, Insn, IsLittleEndian) == Fail)
    return Fail;

  return decodeInstruction(DecoderTable32, MI, Insn, Address, this, STI);
}
