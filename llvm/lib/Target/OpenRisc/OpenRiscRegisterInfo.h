//===-- OpenRiscRegisterInfo.h - OpenRisc Register Information Impl -*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file contains the OpenRisc implementation of the TargetRegisterInfo class.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_OPENRISCREGISTERINFO_H
#define LLVM_LIB_TARGET_OPENRISC_OPENRISCREGISTERINFO_H

#include "OpenRisc.h"
#include "llvm/CodeGen/TargetRegisterInfo.h"

#define GET_REGINFO_HEADER
#include "OpenRiscGenRegisterInfo.inc"

namespace llvm {
class TargetRegisterClass;
class OpenRiscInstrInfo;
class OpenRiscSubtarget;

class OpenRiscRegisterInfo : public OpenRiscGenRegisterInfo {
public:
  const OpenRiscSubtarget &Subtarget;

  OpenRiscRegisterInfo(const OpenRiscSubtarget &STI);

  bool requiresRegisterScavenging(const MachineFunction &MF) const override {
    return true;
  }

  bool requiresFrameIndexScavenging(const MachineFunction &MF) const override {
    return true;
  }

  const uint16_t *
  getCalleeSavedRegs(const MachineFunction *MF = 0) const override;
  const uint32_t *getCallPreservedMask(const MachineFunction &MF,
                                       CallingConv::ID) const override;
  BitVector getReservedRegs(const MachineFunction &MF) const override;

  bool eliminateFrameIndex(MachineBasicBlock::iterator MI, int SPAdj,
                           unsigned FIOperandNum,
                           RegScavenger *RS = nullptr) const override;

  Register getFrameRegister(const MachineFunction &MF) const override;
};

} // end namespace llvm

#endif // LLVM_LIB_TARGET_OPENRISC_REGISTERINFO_H
