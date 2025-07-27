//===-- OR1KMCTargetDesc.h - OR1K Target Descriptions -----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides OR1K specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OR1K_MCTARGETDESC_OR1KMCTARGETDESC_H
#define LLVM_LIB_TARGET_OR1K_MCTARGETDESC_OR1KMCTARGETDESC_H

#include <memory>

namespace llvm {

class MCAsmBackend;
class MCCodeEmitter;
class MCContext;
class MCInstrInfo;
class MCObjectTargetWriter;
class MCObjectWriter;
class MCRegisterInfo;
class MCSubtargetInfo;
class MCTargetOptions;
class StringRef;
class Target;
class raw_ostream;

extern Target TheOR1KTarget;

MCCodeEmitter *createOR1KMCCodeEmitter(const MCInstrInfo &MCII,
                                         MCContext &Ctx);

MCAsmBackend *createOR1KAsmBackend(const Target &T,
                                       const MCSubtargetInfo &STI,
                                       const MCRegisterInfo &MRI,
                                       const MCTargetOptions &Options);
std::unique_ptr<MCObjectTargetWriter>
createOR1KObjectWriter(uint8_t OSABI, bool IsLittleEndian);
} // end namespace llvm

// Defines symbolic names for OR1K registers.
// This defines a mapping from register name to register number.
#define GET_REGINFO_ENUM
#include "OR1KGenRegisterInfo.inc"

// Defines symbolic names for the OR1K instructions.
#define GET_INSTRINFO_ENUM
#include "OR1KGenInstrInfo.inc"

#endif // LLVM_LIB_TARGET_OR1K_MCTARGETDESC_OR1KMCTARGETDESC_H
