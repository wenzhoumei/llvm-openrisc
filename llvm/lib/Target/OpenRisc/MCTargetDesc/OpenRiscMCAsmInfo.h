//===-- OpenRiscMCAsmInfo.h - OpenRisc Asm Info --------------------*- C++ -*--===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the declaration of the OpenRiscMCAsmInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCTARGETASMINFO_H
#define LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCTARGETASMINFO_H

#include "llvm/MC/MCAsmInfoELF.h"

namespace llvm {
class Triple;

class OpenRiscMCAsmInfo : public MCAsmInfoELF {
public:
  explicit OpenRiscMCAsmInfo(const Triple &TT);
};

} // namespace llvm

#endif // LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCTARGETASMINFO_H
