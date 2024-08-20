//===- OpenRiscSubtarget.cpp - OpenRisc Subtarget Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the OpenRisc specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "OpenRiscSubtarget.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "openrisc-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "OpenRiscGenSubtargetInfo.inc"

using namespace llvm;

OpenRiscSubtarget &
OpenRiscSubtarget::initializeSubtargetDependencies(StringRef CPU, StringRef FS) {
  StringRef CPUName = CPU;
  if (CPUName.empty()) {
    // set default cpu name
    CPUName = "generic";
  }

  HasDensity = false;

  // Parse features string.
  ParseSubtargetFeatures(CPUName, CPUName, FS);
  return *this;
}

OpenRiscSubtarget::OpenRiscSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                                 const TargetMachine &TM)
    : OpenRiscGenSubtargetInfo(TT, CPU, /*TuneCPU=*/CPU, FS), TargetTriple(TT),
      InstrInfo(initializeSubtargetDependencies(CPU, FS)), TLInfo(TM, *this),
      TSInfo(), FrameLowering(*this) {}
