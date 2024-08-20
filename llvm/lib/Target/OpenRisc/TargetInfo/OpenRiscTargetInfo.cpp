//===-- OpenRiscTargetInfo.cpp - OpenRisc Target Implementation ---------------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/OpenRiscTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheOpenRiscTarget() {
  static Target TheOpenRiscTarget;
  return TheOpenRiscTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOpenRiscTargetInfo() {
  RegisterTarget<Triple::openrisc> X(getTheOpenRiscTarget(), "openrisc", "OpenRisc 32",
                                   "OPENRISC");
}
