//===- OpenRiscFrameLowering.h - Define frame lowering for OpenRisc --*- C++ -*-==//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===-----------------------------------------------------------------------==//

#ifndef LLVM_LIB_TARGET_OPENRISC_OPENRISCFRAMELOWERING_H
#define LLVM_LIB_TARGET_OPENRISC_OPENRISCFRAMELOWERING_H

#include "llvm/CodeGen/TargetFrameLowering.h"

namespace llvm {
class OpenRiscTargetMachine;
class OpenRiscSubtarget;
class OpenRiscInstrInfo;
class OpenRiscRegisterInfo;

class OpenRiscFrameLowering : public TargetFrameLowering {
  const OpenRiscInstrInfo &TII;
  const OpenRiscRegisterInfo *TRI;

public:
  OpenRiscFrameLowering(const OpenRiscSubtarget &STI);

  bool hasFP(const MachineFunction &MF) const override;

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

#endif /* LLVM_LIB_TARGET_OPENRISC_OPENRISCFRAMELOWERING_H */
