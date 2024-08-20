//===-- OpenRiscMCTargetDesc.cpp - OpenRisc target descriptions ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
#include "OpenRiscMCTargetDesc.h"
#include "TargetInfo/OpenRiscTargetInfo.h"
#include "OpenRiscInstPrinter.h"
#include "OpenRiscMCAsmInfo.h"
#include "OpenRiscTargetStreamer.h"
#include "llvm/ADT/STLExtras.h"
#include "llvm/MC/MCAsmInfo.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCRegisterInfo.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/ErrorHandling.h"

#define GET_INSTRINFO_MC_DESC
#include "OpenRiscGenInstrInfo.inc"

#define GET_REGINFO_MC_DESC
#include "OpenRiscGenRegisterInfo.inc"

#define GET_SUBTARGETINFO_MC_DESC
#include "OpenRiscGenSubtargetInfo.inc"

using namespace llvm;

static MCAsmInfo *createOpenRiscMCAsmInfo(const MCRegisterInfo &MRI,
                                        const Triple &TT,
                                        const MCTargetOptions &Options) {
  MCAsmInfo *MAI = new OpenRiscMCAsmInfo(TT);
  return MAI;
}

static MCInstrInfo *createOpenRiscMCInstrInfo() {
  MCInstrInfo *X = new MCInstrInfo();
  InitOpenRiscMCInstrInfo(X);
  return X;
}

static MCInstPrinter *createOpenRiscMCInstPrinter(const Triple &TT,
                                                unsigned SyntaxVariant,
                                                const MCAsmInfo &MAI,
                                                const MCInstrInfo &MII,
                                                const MCRegisterInfo &MRI) {
  return new OpenRiscInstPrinter(MAI, MII, MRI);
}

static MCRegisterInfo *createOpenRiscMCRegisterInfo(const Triple &TT) {
  MCRegisterInfo *X = new MCRegisterInfo();
  InitOpenRiscMCRegisterInfo(X, OpenRisc::SP);
  return X;
}

static MCSubtargetInfo *
createOpenRiscMCSubtargetInfo(const Triple &TT, StringRef CPU, StringRef FS) {
  return createOpenRiscMCSubtargetInfoImpl(TT, CPU, CPU, FS);
}

static MCTargetStreamer *
createOpenRiscAsmTargetStreamer(MCStreamer &S, formatted_raw_ostream &OS,
                              MCInstPrinter *InstPrint) {
  return new OpenRiscTargetAsmStreamer(S, OS);
}

static MCTargetStreamer *
createOpenRiscObjectTargetStreamer(MCStreamer &S, const MCSubtargetInfo &STI) {
  return new OpenRiscTargetELFStreamer(S);
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOpenRiscTargetMC() {
  // Register the MCAsmInfo.
  TargetRegistry::RegisterMCAsmInfo(getTheOpenRiscTarget(),
                                    createOpenRiscMCAsmInfo);

  // Register the MCCodeEmitter.
  TargetRegistry::RegisterMCCodeEmitter(getTheOpenRiscTarget(),
                                        createOpenRiscMCCodeEmitter);

  // Register the MCInstrInfo.
  TargetRegistry::RegisterMCInstrInfo(getTheOpenRiscTarget(),
                                      createOpenRiscMCInstrInfo);

  // Register the MCInstPrinter.
  TargetRegistry::RegisterMCInstPrinter(getTheOpenRiscTarget(),
                                        createOpenRiscMCInstPrinter);

  // Register the MCRegisterInfo.
  TargetRegistry::RegisterMCRegInfo(getTheOpenRiscTarget(),
                                    createOpenRiscMCRegisterInfo);

  // Register the MCSubtargetInfo.
  TargetRegistry::RegisterMCSubtargetInfo(getTheOpenRiscTarget(),
                                          createOpenRiscMCSubtargetInfo);

  // Register the MCAsmBackend.
  TargetRegistry::RegisterMCAsmBackend(getTheOpenRiscTarget(),
                                       createOpenRiscMCAsmBackend);

  // Register the asm target streamer.
  TargetRegistry::RegisterAsmTargetStreamer(getTheOpenRiscTarget(),
                                            createOpenRiscAsmTargetStreamer);

  // Register the ELF target streamer.
  TargetRegistry::RegisterObjectTargetStreamer(
      getTheOpenRiscTarget(), createOpenRiscObjectTargetStreamer);
}
