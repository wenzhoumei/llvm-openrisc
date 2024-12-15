//===-- OR1KTargetInfo.cpp - OR1K Target Implementation -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TargetInfo/OR1KTargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
using namespace llvm;

Target &llvm::getTheOR1KTarget() {
  static Target TheOR1KTarget;
  return TheOR1KTarget;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOR1KTargetInfo() {
  RegisterTarget<Triple::or1k> X(getTheOR1KTarget(), "or1k", "OR1K",
                                   "OR1K");
}
