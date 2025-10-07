//===- OR1K.h - Top-level interface for OR1K representation -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the entry points for global functions defined in
// the LLVM OR1K back-end.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_OPENRISC_H
#define LLVM_LIB_TARGET_OPENRISC_OPENRISC_H

#include "MCTargetDesc/OR1KMCTargetDesc.h"
#include "llvm/PassRegistry.h"
#include "llvm/Support/CodeGen.h"

namespace llvm {
class OR1KTargetMachine;
class FunctionPass;

FunctionPass *createOR1KISelDag(OR1KTargetMachine &TM,
                                  CodeGenOptLevel OptLevel);
} // namespace llvm
#endif // LLVM_LIB_TARGET_OPENRISC_OPENRISC_H
