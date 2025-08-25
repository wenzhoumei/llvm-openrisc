//===- OR1KSubtarget.cpp - OR1K Subtarget Information -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the OR1K specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#include "OR1KSubtarget.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/Support/Debug.h"

#define DEBUG_TYPE "OR1K-subtarget"

#define GET_SUBTARGETINFO_TARGET_DESC
#define GET_SUBTARGETINFO_CTOR
#include "OR1KGenSubtargetInfo.inc"

using namespace llvm;

OR1KSubtarget::OR1KSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                             const OR1KTargetMachine &TM)
    : OR1KGenSubtargetInfo(TT,
                           CPU.empty() ? StringRef("generic") : CPU,
                           /*TuneCPU=*/CPU.empty() ? StringRef("generic") : CPU,
                           FS) {
  ParseSubtargetFeatures(CPU.empty() ? StringRef("generic") : CPU,
                         CPU.empty() ? StringRef("generic") : CPU, FS);
}
