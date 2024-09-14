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
  if (MI.getOpcode() == OpenRisc::LWS) {
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
  // If machine instruction is storing a word
  if (MI.getOpcode() == OpenRisc::SW) {
    if (MI.getOperand(1).isFI() && MI.getOperand(2).isImm() &&
        MI.getOperand(2).getImm() == 0) {
      FrameIndex = MI.getOperand(1).getIndex();
      return MI.getOperand(0).getReg();
    }
  }
  return Register();
}

// Adjust SP by Amount bytes.
void OpenRiscInstrInfo::adjustStackPtr(unsigned SP, int64_t Amount,
                                     MachineBasicBlock &MBB,
                                     MachineBasicBlock::iterator I) const {
  DebugLoc DL = I != MBB.end() ? I->getDebugLoc() : DebugLoc();

  if (Amount == 0)
    return;

  MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RC = &OpenRisc::GPRRegClass;

  // create virtual reg to store immediate
  unsigned Reg = RegInfo.createVirtualRegister(RC);

  // If immediate is 16-bit, directly add it to stack pointer
  if (isInt<16>(Amount)) { // addi sp, sp, amount
    BuildMI(MBB, I, DL, get(OpenRisc::ADDI), Reg).addReg(SP).addImm(Amount);
  } else { // Expand immediate that doesn't fit in 16-bit.
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
                                  MCRegister SrcReg, bool KillSrc,
                                  bool RenamableDest, bool RenamableSrc) const {
  // The MOV instruction is not present in core ISA,
  // so use OR instruction.
  if (OpenRisc::GPRRegClass.contains(DestReg, SrcReg))
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
  assert((RC == &OpenRisc::GPRRegClass) &&
         "Unsupported regclass to load or store");

  LoadOpcode = OpenRisc::LWS;
  StoreOpcode = OpenRisc::SW;
}

void OpenRiscInstrInfo::loadImmediate(MachineBasicBlock &MBB,
                                    MachineBasicBlock::iterator MBBI,
                                    unsigned *Reg, int64_t Value) const {
  DebugLoc DL = MBBI != MBB.end() ? MBBI->getDebugLoc() : DebugLoc();
  MachineRegisterInfo &RegInfo = MBB.getParent()->getRegInfo();
  const TargetRegisterClass *RC = &OpenRisc::GPRRegClass;

  // create virtual reg to store immediate
  *Reg = RegInfo.createVirtualRegister(RC);
  if (Value >= -32768 && Value <= 32767) { // if 16 bits
    // Use ADDI with zero register (R0) as the source
    BuildMI(MBB, MBBI, DL, get(OpenRisc::ADDI), *Reg)
        .addReg(OpenRisc::R0)    // R0 as the source register
        .addImm(Value);          // imm16 immediate value
  } else if (Value >= -4294967296LL && Value <= 4294967295LL) {// 32 bit arbitrary constant

    int Low = Value & 0xFFFF;
    int High = Value & ~0xFFFF;

    BuildMI(MBB, MBBI, DL, get(OpenRisc::MOVHI), *Reg).addImm(High);
    BuildMI(MBB, MBBI, DL, get(OpenRisc::ADDI), *Reg).addReg(*Reg).addImm(Low);
  } else {
    // TODO support values greater than 32 bits
    report_fatal_error("Unsupported load immediate value");
  }
}
