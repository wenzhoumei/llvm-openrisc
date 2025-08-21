//===-- OR1KMCTargetDesc.cpp - OR1K Target Descriptions -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "OR1KMCTargetDesc.h"
#include "OR1KMCAsmInfo.h"
#include "OR1KInstPrinter.h"
#include "TargetInfo/OR1KTargetInfo.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/TargetRegistry.h"

#define GET_INSTRINFO_MC_DESC
#include "OR1KGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "OR1KGenRegisterInfo.inc"

using namespace llvm;

static MCAsmInfo *createOR1KMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new OR1KMCAsmInfo(TT);
  return MAI;
}

static MCInstrInfo *createOR1KMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitOR1KMCInstrInfo(X);
  return X;
}

static MCInstPrinter *createOR1KMCInstPrinter(const Triple &TT,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  return new OR1KInstPrinter(MAI, MII, MRI);
}

static MCRegisterInfo *createOR1KMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitOR1KMCRegisterInfo(X, OR1K::R1);
  return X;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOR1KTargetMC() {
  // Register the MCAsmInfo.
  TargetRegistry::RegisterMCAsmInfo(getTheOR1KTarget(), createOR1KMCAsmInfo);

  // Register the MCCodeEmitter.
  TargetRegistry::RegisterMCCodeEmitter(getTheOR1KTarget(),
                                        createOR1KMCCodeEmitter);

  // Register the MCInstrInfo.
  TargetRegistry::RegisterMCInstrInfo(getTheOR1KTarget(), createOR1KMCInstrInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheOR1KTarget(),
                                        createOR1KMCInstPrinter);

  // Register the MCRegisterInfo.
  TargetRegistry::RegisterMCRegInfo(getTheOR1KTarget(),
                                    createOR1KMCRegisterInfo);

  // Register the MCAsmBackend.
  TargetRegistry::RegisterMCAsmBackend(getTheOR1KTarget(),
                                       createOR1KAsmBackend);
}
