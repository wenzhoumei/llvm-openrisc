//===- OR1KFrameLowering.h - Define frame lowering for OR1K --*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===-----------------------------------------------------------------------==//

#ifndef LLVM_LIB_TARGET_OR1K_OR1KFRAMELOWERING_H
#define LLVM_LIB_TARGET_OR1K_OR1KFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class OR1KTargetMachine;
class OR1KSubtarget;
class OR1KInstrInfo;
class OR1KRegisterInfo;

class OR1KFrameLowering : public TargetFrameLowering {
  const OR1KSubtarget &STI;
  const OR1KInstrInfo &TII;
  const OR1KRegisterInfo *TRI;

  bool hasFPImpl(const MachineFunction &MF) const override;
public:
  OR1KFrameLowering(const OR1KSubtarget &STI);

  /// emitProlog/emitEpilog - These methods insert prolog and epilog code into
  /// the function.
  void emitPrologue(MachineFunction &, MachineBasicBlock &) const override;
  void emitEpilogue(MachineFunction &MF, MachineBasicBlock &MBB) const override;

  MachineBasicBlock::iterator
  eliminateCallFramePseudoInstr(MachineFunction &MF, MachineBasicBlock &MBB,
                                MachineBasicBlock::iterator I) const override;

  bool spillCalleeSavedRegisters(MachineBasicBlock &MBB,
                                 MachineBasicBlock::iterator MI,
                                 ArrayRef<CalleeSavedInfo> CSI,
                                 const TargetRegisterInfo *TRI) const override;
  bool
  restoreCalleeSavedRegisters(MachineBasicBlock &MBB,
                              MachineBasicBlock::iterator MI,
                              MutableArrayRef<CalleeSavedInfo> CSI,
                              const TargetRegisterInfo *TRI) const override;

  void determineCalleeSaves(MachineFunction &MF, BitVector &SavedRegs,
                            RegScavenger *RS) const override;
};

} // namespace llvm

#endif /* LLVM_LIB_TARGET_OR1K_OR1KFRAMELOWERING_H */
