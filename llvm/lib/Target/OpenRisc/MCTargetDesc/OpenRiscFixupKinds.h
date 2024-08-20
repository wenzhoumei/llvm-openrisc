//===-- OpenRiscMCFixups.h - OpenRisc-specific fixup entries --------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCMCFIXUPS_H
#define LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCMCFIXUPS_H

#include "llvm/MC/MCFixup.h"

namespace llvm {
namespace OpenRisc {
enum FixupKind {
  fixup_openrisc_branch_6 = FirstTargetFixupKind,
  fixup_openrisc_branch_8,
  fixup_openrisc_branch_12,
  fixup_openrisc_jump_18,
  fixup_openrisc_call_18,
  fixup_openrisc_l32r_16,
  fixup_openrisc_invalid,
  LastTargetFixupKind,
  NumTargetFixupKinds = LastTargetFixupKind - FirstTargetFixupKind
};
} // end namespace OpenRisc
} // end namespace llvm

#endif // LLVM_LIB_TARGET_OPENRISC_MCTARGETDESC_OPENRISCMCFIXUPS_H
