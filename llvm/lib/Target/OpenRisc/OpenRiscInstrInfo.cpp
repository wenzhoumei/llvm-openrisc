//===- OpenRiscInstrInfo.cpp - OpenRisc Instruction Information ---------------===//
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

#include "OpenRiscInstrInfo.h"
#include "OpenRiscTargetMachine.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"

#define GET_INSTRINFO_CTOR_DTOR
#include "OpenRiscGenInstrInfo.inc"

using namespace llvm;

static const MachineInstrBuilder &
addFrameReference(const MachineInstrBuilder &MIB, int FI) {
  MachineInstr *MI = MIB;
  MachineFunction &MF = *MI->getParent()->getParent();
  MachineFrameInfo &MFFrame = MF.getFrameInfo();
  const MCInstrDesc &MCID = MI->getDesc();
  MachineMemOperand::Flags Flags = MachineMemOperand::MONone;
  if (MCID.mayLoad())
    Flags |= MachineMemOperand::MOLoad;
  if (MCID.mayStore())
    Flags |= MachineMemOperand::MOStore;
  int64_t Offset = 0;
  Align Alignment = MFFrame.getObjectAlign(FI);

  MachineMemOperand *MMO =
      MF.getMachineMemOperand(MachinePointerInfo::getFixedStack(MF, FI, Offset),
                              Flags, MFFrame.getObjectSize(FI), Alignment);
  return MIB.addFrameIndex(FI).addImm(Offset).addMemOperand(MMO);
}

OpenRiscInstrInfo::OpenRiscInstrInfo(const OpenRiscSubtarget &STI)
    : OpenRiscGenInstrInfo(OpenRisc::ADJCALLSTACKDOWN, OpenRisc::ADJCALLSTACKUP),
      RI(STI), STI(STI) {}

Register OpenRiscInstrInfo::isLoadFromStackSlot(const MachineInstr &MI,
                                              int &FrameIndex) const {
  if (MI.getOpcode() == OpenRisc::L32I) {
    if (MI.getOperand(1).isFI() && MI.getOperand(2).isImm() &&
        MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
  }
  return Register();
}

Register OpenRiscInstrInfo::isStoreToStackSlot(const MachineInstr &MI,
                                             int &FrameIndex) const {
  if (MI.getOpcode() == OpenRisc::S32I) {
    if (MI.getOperand(1).isFI() && MI.getOperand(2).isImm() &&
        MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
  }
  return Register();
}

/// Adjust SP by Amount bytes.
void OpenRiscInstrInfo::adjustStackPtr(unsigned SP, int64_t Amount,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const {
  DebugLoc DL = I != MBB.end() ? I->getDebugLoc() : DebugLoc();

  if (Amount == 0)
    return;

  MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RC = &OpenRisc::ARRegClass;

  // create virtual reg to store immediate
  unsigned Reg = RegInfo.createVirtualRegister(RC);

  if (isInt<8>(Amount)) { // addi sp, sp, amount
    BuildMI(MBB, I, DL, get(OpenRisc::ADDI), Reg).addReg(SP).addImm(Amount);
  } else { // Expand immediate that doesn't fit in 8-bit.
    unsigned Reg1;
    loadImmediate(MBB, I, &Reg1, Amount);
    BuildMI(MBB, I, DL, get(OpenRisc::ADD), Reg)
        .addReg(SP)
        .addReg(Reg1, RegState::Kill);
  }

  BuildMI(MBB, I, DL, get(OpenRisc::OR), SP)
      .addReg(Reg, RegState::Kill)
      .addReg(Reg, RegState::Kill);
}

void OpenRiscInstrInfo::copyPhysReg(MachineBasicBlock &MBB,
                                  MachineBasicBlock::iterator MBBI,
                                  const DebugLoc &DL, MCRegister DestReg,
                                  MCRegister SrcReg, bool KillSrc) const {
  // The MOV instruction is not present in core ISA,
  // so use OR instruction.
  if (OpenRisc::ARRegClass.contains(DestReg, SrcReg))
    BuildMI(MBB, MBBI, DL, get(OpenRisc::OR), DestReg)
        .addReg(SrcReg, getKillRegState(KillSrc))
        .addReg(SrcReg, getKillRegState(KillSrc));
  else
    report_fatal_error("Impossible reg-to-reg copy");
}

void OpenRiscInstrInfo::storeRegToStackSlot(
    MachineBasicBlock &MBB, MachineBasicBlock::iterator MBBI, Register SrcReg,
    bool isKill, int FrameIdx, const TargetRegisterClass *RC,
    const TargetRegisterInfo *TRI, Register VReg) const {
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  unsigned LoadOpcode, StoreOpcode;
  getLoadStoreOpcodes(RC, LoadOpcode, StoreOpcode, FrameIdx);
  MachineInstrBuilder MIB = BuildMI(MBB, MBBI, DL, get(StoreOpcode))
                                .addReg(SrcReg, getKillRegState(isKill));
  addFrameReference(MIB, FrameIdx);
}

void OpenRiscInstrInfo::loadRegFromStackSlot(MachineBasicBlock &MBB,
                                           MachineBasicBlock::iterator MBBI,
                                           Register DestReg, int FrameIdx,
                                           const TargetRegisterClass *RC,
                                           const TargetRegisterInfo *TRI,
                                           Register VReg) const {
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  unsigned LoadOpcode, StoreOpcode;
  getLoadStoreOpcodes(RC, LoadOpcode, StoreOpcode, FrameIdx);
  addFrameReference(BuildMI(MBB, MBBI, DL, get(LoadOpcode), DestReg), FrameIdx);
}

void OpenRiscInstrInfo::getLoadStoreOpcodes(const TargetRegisterClass *RC,
                                          unsigned &LoadOpcode,
                                          unsigned &StoreOpcode,
                                          int64_t offset) const {
  assert((RC == &OpenRisc::ARRegClass) &&
         "Unsupported regclass to load or store");

  LoadOpcode = OpenRisc::L32I;
  StoreOpcode = OpenRisc::S32I;
}

void OpenRiscInstrInfo::loadImmediate(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    unsigned *Reg, int64_t Value) const {
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RC = &OpenRisc::ARRegClass;

  // create virtual reg to store immediate
  *Reg = RegInfo.createVirtualRegister(RC);
  if (Value >= -2048 && Value <= 2047) {
    BuildMI(MBB, MBBI, DL, get(OpenRisc::MOVI), *Reg).addImm(Value);
  } else if (Value >= -32768 && Value <= 32767) {
    int Low = Value & 0xFF;
    int High = Value & ~0xFF;

    BuildMI(MBB, MBBI, DL, get(OpenRisc::MOVI), *Reg).addImm(Low);
    BuildMI(MBB, MBBI, DL, get(OpenRisc::ADDMI), *Reg).addReg(*Reg).addImm(High);
  } else if (Value >= -4294967296LL && Value <= 4294967295LL) {
    // 32 bit arbitrary constant
    // TODO: Implement a constant pool
    report_fatal_error("Currently unsupported load immediate value due to lack of implementing a constant pool");
  } else {
    // use L32R to let assembler load immediate best
    // TODO replace to L32R
    report_fatal_error("Unsupported load immediate value");
  }
}
