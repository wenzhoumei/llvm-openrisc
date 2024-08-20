//===-- OpenRiscMCTargetDesc.h - OpenRisc Target Descriptions -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file provides OpenRisc specific target descriptions.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCMCTARGETDESC_H
#define LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCMCTARGETDESC_H
#include "llvm/Support/DataTypes.h"
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

extern Target TheOpenRiscTarget;

MCCodeEmitter *createOpenRiscMCCodeEmitter(const MCInstrInfo &MCII,
                                         MCContext &Ctx);

MCAsmBackend *createOpenRiscMCAsmBackend(const Target &T,
                                       const MCSubtargetInfo &STI,
                                       const MCRegisterInfo &MRI,
                                       const MCTargetOptions &Options);
std::unique_ptr<MCObjectTargetWriter>
createOpenRiscObjectWriter(uint8_t OSABI, bool IsLittleEndian);
} // end namespace llvm

// Defines symbolic names for OpenRisc registers.
// This defines a mapping from register name to register number.
#define GET_REGINFO_ENUM
#include "OpenRiscGenRegisterInfo.inc"

// Defines symbolic names for the OpenRisc instructions.
#define GET_INSTRINFO_ENUM
#include "OpenRiscGenInstrInfo.inc"

#define GET_SUBTARGETINFO_ENUM
#include "OpenRiscGenSubtargetInfo.inc"

#endif // LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCMCTARGETDESC_H
