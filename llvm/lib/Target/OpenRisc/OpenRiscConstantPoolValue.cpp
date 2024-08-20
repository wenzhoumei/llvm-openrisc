//===- OpenRiscConstantPoolValue.cpp - OpenRisc constantpool value ------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the OpenRisc specific constantpool value class.
//
//===----------------------------------------------------------------------===//

#include "OpenRiscConstantPoolValue.h"
#include "llvm/ADT/FoldingSet.h"
#include "llvm/CodeGen/MachineBasicBlock.h"
#include "llvm/IR/Constant.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/GlobalValue.h"
#include "llvm/IR/Type.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
using namespace llvm;

OpenRiscConstantPoolValue::OpenRiscConstantPoolValue(
    Type *Ty, unsigned ID, OpenRiscCP::OpenRiscCPKind Kind,
    OpenRiscCP::OpenRiscCPModifier modifier)
    : MachineConstantPoolValue(Ty), LabelId(ID), Kind(Kind),
      Modifier(modifier) {}

OpenRiscConstantPoolValue::OpenRiscConstantPoolValue(
    LLVMContext &C, unsigned ID, OpenRiscCP::OpenRiscCPKind Kind,
    OpenRiscCP::OpenRiscCPModifier Modifier)
    : MachineConstantPoolValue((Type *)Type::getInt32Ty(C)), LabelId(ID),
      Kind(Kind), Modifier(Modifier) {}

OpenRiscConstantPoolValue::~OpenRiscConstantPoolValue() {}

StringRef OpenRiscConstantPoolValue::getModifierText() const {
  switch (Modifier) {
  case OpenRiscCP::no_modifier:
    return "";
  case OpenRiscCP::TPOFF:
    return "@TPOFF";
  }
  report_fatal_error("Unknown modifier!");
}

int OpenRiscConstantPoolValue::getExistingMachineCPValue(MachineConstantPool *CP,
                                                       Align Alignment) {
  report_fatal_error("Shouldn't be calling this directly!");
}

void OpenRiscConstantPoolValue::addSelectionDAGCSEId(FoldingSetNodeID &ID) {
  ID.AddInteger(LabelId);
}

bool OpenRiscConstantPoolValue::hasSameValue(OpenRiscConstantPoolValue *ACPV) {
  if (ACPV->Kind == Kind) {
    if (ACPV->LabelId == LabelId)
      return true;
  }
  return false;
}

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
void OpenRiscConstantPoolValue::dump() const { errs() << "  " << *this; }
#endif

void OpenRiscConstantPoolValue::print(raw_ostream &O) const {}

//===----------------------------------------------------------------------===//
// OpenRiscConstantPoolConstant
//===----------------------------------------------------------------------===//

OpenRiscConstantPoolConstant::OpenRiscConstantPoolConstant(
    const Constant *C, unsigned ID, OpenRiscCP::OpenRiscCPKind Kind)
    : OpenRiscConstantPoolValue(C->getType(), ID, Kind), CVal(C) {}

OpenRiscConstantPoolConstant *
OpenRiscConstantPoolConstant::Create(const Constant *C, unsigned ID,
                                   OpenRiscCP::OpenRiscCPKind Kind) {
  return new OpenRiscConstantPoolConstant(C, ID, Kind);
}

const BlockAddress *OpenRiscConstantPoolConstant::getBlockAddress() const {
  return dyn_cast_or_null<BlockAddress>(CVal);
}

int OpenRiscConstantPoolConstant::getExistingMachineCPValue(
    MachineConstantPool *CP, Align Alignment) {
  return getExistingMachineCPValueImpl<OpenRiscConstantPoolConstant>(CP,
                                                                   Alignment);
}

bool OpenRiscConstantPoolConstant::hasSameValue(OpenRiscConstantPoolValue *ACPV) {
  const OpenRiscConstantPoolConstant *ACPC =
      dyn_cast<OpenRiscConstantPoolConstant>(ACPV);
  return ACPC && ACPC->CVal == CVal &&
         OpenRiscConstantPoolValue::hasSameValue(ACPV);
}

void OpenRiscConstantPoolConstant::addSelectionDAGCSEId(FoldingSetNodeID &ID) {
  ID.AddPointer(CVal);
  OpenRiscConstantPoolValue::addSelectionDAGCSEId(ID);
}

void OpenRiscConstantPoolConstant::print(raw_ostream &O) const {
  O << CVal->getName();
  OpenRiscConstantPoolValue::print(O);
}

OpenRiscConstantPoolSymbol::OpenRiscConstantPoolSymbol(
    LLVMContext &C, const char *Str, unsigned ID, bool PrivLinkage,
    OpenRiscCP::OpenRiscCPModifier Modifier)
    : OpenRiscConstantPoolValue(C, ID, OpenRiscCP::CPExtSymbol, Modifier), S(Str),
      PrivateLinkage(PrivLinkage) {}

OpenRiscConstantPoolSymbol *
OpenRiscConstantPoolSymbol::Create(LLVMContext &C, const char *Str, unsigned ID,
                                 bool PrivLinkage,
                                 OpenRiscCP::OpenRiscCPModifier Modifier)

{
  return new OpenRiscConstantPoolSymbol(C, Str, ID, PrivLinkage, Modifier);
}

int OpenRiscConstantPoolSymbol::getExistingMachineCPValue(MachineConstantPool *CP,
                                                        Align Alignment) {
  return getExistingMachineCPValueImpl<OpenRiscConstantPoolSymbol>(CP, Alignment);
}

bool OpenRiscConstantPoolSymbol::hasSameValue(OpenRiscConstantPoolValue *ACPV) {
  const OpenRiscConstantPoolSymbol *ACPS =
      dyn_cast<OpenRiscConstantPoolSymbol>(ACPV);
  return ACPS && ACPS->S == S && OpenRiscConstantPoolValue::hasSameValue(ACPV);
}

void OpenRiscConstantPoolSymbol::addSelectionDAGCSEId(FoldingSetNodeID &ID) {
  ID.AddString(S);
  OpenRiscConstantPoolValue::addSelectionDAGCSEId(ID);
}

void OpenRiscConstantPoolSymbol::print(raw_ostream &O) const {
  O << S;
  OpenRiscConstantPoolValue::print(O);
}

OpenRiscConstantPoolMBB::OpenRiscConstantPoolMBB(LLVMContext &C,
                                             const MachineBasicBlock *M,
                                             unsigned Id)
    : OpenRiscConstantPoolValue(C, 0, OpenRiscCP::CPMachineBasicBlock), MBB(M) {}

OpenRiscConstantPoolMBB *OpenRiscConstantPoolMBB::Create(LLVMContext &C,
                                                     const MachineBasicBlock *M,
                                                     unsigned Idx) {
  return new OpenRiscConstantPoolMBB(C, M, Idx);
}

int OpenRiscConstantPoolMBB::getExistingMachineCPValue(MachineConstantPool *CP,
                                                     Align Alignment) {
  return getExistingMachineCPValueImpl<OpenRiscConstantPoolMBB>(CP, Alignment);
}

bool OpenRiscConstantPoolMBB::hasSameValue(OpenRiscConstantPoolValue *ACPV) {
  const OpenRiscConstantPoolMBB *ACPMBB = dyn_cast<OpenRiscConstantPoolMBB>(ACPV);
  return ACPMBB && ACPMBB->MBB == MBB &&
         OpenRiscConstantPoolValue::hasSameValue(ACPV);
}

void OpenRiscConstantPoolMBB::addSelectionDAGCSEId(FoldingSetNodeID &ID) {
  ID.AddPointer(MBB);
  OpenRiscConstantPoolValue::addSelectionDAGCSEId(ID);
}

void OpenRiscConstantPoolMBB::print(raw_ostream &O) const {
  O << "BB#" << MBB->getNumber();
  OpenRiscConstantPoolValue::print(O);
}

OpenRiscConstantPoolJumpTable::OpenRiscConstantPoolJumpTable(LLVMContext &C,
                                                         unsigned Index)
    : OpenRiscConstantPoolValue(C, 0, OpenRiscCP::CPJumpTable), Idx(Index) {}

OpenRiscConstantPoolJumpTable *OpenRiscConstantPoolJumpTable::Create(LLVMContext &C,
                                                                 unsigned Idx) {
  return new OpenRiscConstantPoolJumpTable(C, Idx);
}

int OpenRiscConstantPoolJumpTable::getExistingMachineCPValue(
    MachineConstantPool *CP, Align Alignment) {
  return getExistingMachineCPValueImpl<OpenRiscConstantPoolJumpTable>(CP,
                                                                    Alignment);
}

bool OpenRiscConstantPoolJumpTable::hasSameValue(OpenRiscConstantPoolValue *ACPV) {
  const OpenRiscConstantPoolJumpTable *ACPJT =
      dyn_cast<OpenRiscConstantPoolJumpTable>(ACPV);
  return ACPJT && ACPJT->Idx == Idx &&
         OpenRiscConstantPoolValue::hasSameValue(ACPV);
}

void OpenRiscConstantPoolJumpTable::addSelectionDAGCSEId(FoldingSetNodeID &ID) {}

void OpenRiscConstantPoolJumpTable::print(raw_ostream &O) const {
  O << "JT" << Idx;
  OpenRiscConstantPoolValue::print(O);
}
