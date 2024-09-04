//===--- OpenRiscUtils.cpp ---- OpenRisc Utility Functions ----------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains miscellaneous utility functions.
//
//===----------------------------------------------------------------------===//

#include "OpenRiscUtils.h"

namespace llvm {

bool isValidAddrOffset(MachineInstr &MI, int64_t Offset) {
  return (Offset >= -32768 && Offset <= 32767);
}

} // namespace llvm
