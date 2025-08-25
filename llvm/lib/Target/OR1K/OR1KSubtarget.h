//===-- OR1KSubtarget.h - Define Subtarget for the OR1K ----*- C++ -*--===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the OR1K specific subclass of TargetSubtargetInfo.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OR1K_OR1KSUBTARGET_H
#define LLVM_LIB_TARGET_OR1K_OR1KSUBTARGET_H

#include "llvm/CodeGen/TargetSubtargetInfo.h"

#define GET_SUBTARGETINFO_HEADER
#include "OR1KGenSubtargetInfo.inc"

namespace llvm {

class OR1KTargetMachine;

class OR1KSubtarget : public OR1KGenSubtargetInfo {
public:
  OR1KSubtarget(const Triple &TT, StringRef CPU, StringRef FS,
                const OR1KTargetMachine &TM);

  // tblgen will generate this parser.
  void ParseSubtargetFeatures(StringRef CPU, StringRef TuneCPU, StringRef FS);
};

} // end namespace llvm

#endif
