//===-- OR1KMCFixups.h - OR1K-specific fixup entries ------------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OR1K_MCTARGETDESC_OR1KMCFIXUPS_H
#define LLVM_LIB_TARGET_OR1K_MCTARGETDESC_OR1KMCFIXUPS_H

#include "llvm/MC/MCFixup.h"

namespace llvm::OR1K {
enum Fixups {
  fixup_or1k_branch = FirstTargetFixupKind,
  fixup_or1k_hi16,
  fixup_or1k_lo16,
  fixup_or1k_invalid,
  NumTargetFixupKinds = fixup_or1k_invalid - FirstTargetFixupKind
};

} // end namespace llvm::OR1K

#endif // LLVM_LIB_TARGET_OR1K_MCTARGETDESC_OR1KMCFIXUPS_H
