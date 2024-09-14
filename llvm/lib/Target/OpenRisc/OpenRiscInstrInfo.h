//===-- OpenRiscInstrInfo.h - OpenRisc Instruction Information ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the OpenRisc implementation of the TargetInstrInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_OPENRISCINSTRINFO_H
#define LLVM_LIB_TARGET_OPENRISC_OPENRISCINSTRINFO_H

#include "OpenRisc.h"
#include "OpenRiscRegisterInfo.h"
#include "llvm/CodeGen/TargetInstrInfo.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_INSTRINFO_HEADER

#include "OpenRiscGenInstrInfo.inc"

namespace llvm {

class OpenRiscTargetMachine;
class OpenRiscSubtarget;
class OpenRiscInstrInfo : public OpenRiscGenInstrInfo {
  const OpenRiscRegisterInfo RI;
  const OpenRiscSubtarget &STI;

public:
  OpenRiscInstrInfo(const OpenRiscSubtarget &STI);

  void adjustStackPtr(unsigned SP, int64_t Amount, MachineBasicBlock &MBB,
                      MachineBasicBlock::iterator I) const;

  // Return the OpenRiscRegisterInfo, which this class owns.
  const OpenRiscRegisterInfo &getRegisterInfo() const { return RI; }

  Register isLoadFromStackSlot(const MachineInstr &MI,
                               int &FrameIndex) const override;

  Register isStoreToStackSlot(const MachineInstr &MI,
                              int &FrameIndex) const override;

  void copyPhysReg(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                   const DebugLoc &DL, MCRegister DestReg, MCRegister SrcReg,
                   bool KillSrc, bool RenamableDest = false,
                   bool RenamableSrc = false) const override;

  void storeRegToStackSlot(MachineBasicBlock &MBB,
                           MachineBasicBlock::iterator MBBI, Register SrcReg,
                           bool isKill, int FrameIndex,
                           const TargetRegisterClass *RC,
                           const TargetRegisterInfo *TRI,
                           Register VReg) const override;

  void loadRegFromStackSlot(MachineBasicBlock &MBB,
                            MachineBasicBlock::iterator MBBI, Register DestReg,
                            int FrameIdx, const TargetRegisterClass *RC,
                            const TargetRegisterInfo *TRI,
                            Register VReg) const override;

  // Get the load and store opcodes for a given register class and offset.
  void getLoadStoreOpcodes(const TargetRegisterClass *RC, unsigned &LoadOpcode,
                           unsigned &StoreOpcode, int64_t offset) const;

  // Emit code before MBBI in MI to move immediate value Value into
  // physical register Reg.
  void loadImmediate(MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI,
                     unsigned *Reg, int64_t Value) const;

  const OpenRiscSubtarget &getSubtarget() const { return STI; }
};
} // end namespace llvm

#endif /* LLVM_LIB_TARGET_OPENRISC_OPENRISCINSTRINFO_H */
