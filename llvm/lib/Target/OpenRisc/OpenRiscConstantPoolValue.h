//===- OpenRiscConstantPoolValue.h - OpenRisc constantpool value ----*- C++ -*-===//
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

#ifndef LLVM_LIB_TARGET_OPENRISC_OPENRISCCONSTANTPOOLVALUE_H
#define LLVM_LIB_TARGET_OPENRISC_OPENRISCCONSTANTPOOLVALUE_H

#include "llvm/CodeGen/MachineConstantPool.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/ErrorHandling.h"
#include <cstddef>
#include <string>
#include <vector>

namespace llvm {

class BlockAddress;
class Constant;
class GlobalValue;
class LLVMContext;
class MachineBasicBlock;

namespace OpenRiscCP {
enum OpenRiscCPKind {
  CPExtSymbol,
  CPBlockAddress,
  CPMachineBasicBlock,
  CPJumpTable
};

enum OpenRiscCPModifier {
  no_modifier, // None
  TPOFF        // Thread Pointer Offset
};
} // namespace OpenRiscCP

/// OpenRiscConstantPoolValue - OpenRisc specific constantpool value. This is used
/// to represent PC-relative displacement between the address of the load
/// instruction and the constant being loaded.
class OpenRiscConstantPoolValue : public MachineConstantPoolValue {
  unsigned LabelId;                    // Label id of the load.
  OpenRiscCP::OpenRiscCPKind Kind;         // Kind of constant.
  OpenRiscCP::OpenRiscCPModifier Modifier; // Symbol name modifier
                                       //(for example Global Variable name)

protected:
  OpenRiscConstantPoolValue(
      Type *Ty, unsigned ID, OpenRiscCP::OpenRiscCPKind Kind,
      OpenRiscCP::OpenRiscCPModifier Modifier = OpenRiscCP::no_modifier);

  OpenRiscConstantPoolValue(
      LLVMContext &C, unsigned id, OpenRiscCP::OpenRiscCPKind Kind,
      OpenRiscCP::OpenRiscCPModifier Modifier = OpenRiscCP::no_modifier);

  template <typename Derived>
  int getExistingMachineCPValueImpl(MachineConstantPool *CP, Align Alignment) {
    const std::vector<MachineConstantPoolEntry> &Constants = CP->getConstants();
    for (unsigned i = 0, e = Constants.size(); i != e; ++i) {
      if (Constants[i].isMachineConstantPoolEntry() &&
          (Constants[i].getAlign() >= Alignment)) {
        auto *CPV = static_cast<OpenRiscConstantPoolValue *>(
            Constants[i].Val.MachineCPVal);
        if (Derived *APC = dyn_cast<Derived>(CPV))
          if (cast<Derived>(this)->equals(APC))
            return i;
      }
    }

    return -1;
  }

public:
  ~OpenRiscConstantPoolValue() override;

  OpenRiscCP::OpenRiscCPModifier getModifier() const { return Modifier; }
  bool hasModifier() const { return Modifier != OpenRiscCP::no_modifier; }
  StringRef getModifierText() const;

  unsigned getLabelId() const { return LabelId; }
  void setLabelId(unsigned ID) { LabelId = ID; }

  bool isExtSymbol() const { return Kind == OpenRiscCP::CPExtSymbol; }
  bool isBlockAddress() const { return Kind == OpenRiscCP::CPBlockAddress; }
  bool isMachineBasicBlock() const {
    return Kind == OpenRiscCP::CPMachineBasicBlock;
  }
  bool isJumpTable() const { return Kind == OpenRiscCP::CPJumpTable; }

  int getExistingMachineCPValue(MachineConstantPool *CP,
                                Align Alignment) override;

  void addSelectionDAGCSEId(FoldingSetNodeID &ID) override;

  /// hasSameValue - Return true if this OpenRisc constpool value can share the
  /// same constantpool entry as another OpenRisc constpool value.
  virtual bool hasSameValue(OpenRiscConstantPoolValue *ACPV);

  bool equals(const OpenRiscConstantPoolValue *A) const {
    return this->LabelId == A->LabelId && this->Modifier == A->Modifier;
  }

  void print(raw_ostream &O) const override;

#if !defined(NDEBUG) || defined(LLVM_ENABLE_DUMP)
  void dump() const;
#endif
};

inline raw_ostream &operator<<(raw_ostream &O,
                               const OpenRiscConstantPoolValue &V) {
  V.print(O);
  return O;
}

/// OpenRiscConstantPoolConstant - OpenRisc-specific constant pool values for
/// Constants (for example BlockAddresses).
class OpenRiscConstantPoolConstant : public OpenRiscConstantPoolValue {
  const Constant *CVal; // Constant being loaded.

  OpenRiscConstantPoolConstant(const Constant *C, unsigned ID,
                             OpenRiscCP::OpenRiscCPKind Kind);

public:
  static OpenRiscConstantPoolConstant *Create(const Constant *C, unsigned ID,
                                            OpenRiscCP::OpenRiscCPKind Kind);

  const BlockAddress *getBlockAddress() const;

  int getExistingMachineCPValue(MachineConstantPool *CP,
                                Align Alignment) override;

  /// hasSameValue - Return true if this OpenRisc constpool value can share the
  /// same constantpool entry as another OpenRisc constpool value.
  bool hasSameValue(OpenRiscConstantPoolValue *ACPV) override;

  void addSelectionDAGCSEId(FoldingSetNodeID &ID) override;

  void print(raw_ostream &O) const override;
  static bool classof(const OpenRiscConstantPoolValue *APV) {
    return APV->isBlockAddress();
  }

  bool equals(const OpenRiscConstantPoolConstant *A) const {
    return CVal == A->CVal && OpenRiscConstantPoolValue::equals(A);
  }
};

/// OpenRiscConstantPoolSymbol - OpenRisc-specific constantpool values for external
/// symbols.
class OpenRiscConstantPoolSymbol : public OpenRiscConstantPoolValue {
  const std::string S; // ExtSymbol being loaded.
  bool PrivateLinkage;

  OpenRiscConstantPoolSymbol(
      LLVMContext &C, const char *S, unsigned Id, bool PrivLinkage,
      OpenRiscCP::OpenRiscCPModifier Modifier = OpenRiscCP::no_modifier);

public:
  static OpenRiscConstantPoolSymbol *
  Create(LLVMContext &C, const char *S, unsigned ID, bool PrivLinkage,
         OpenRiscCP::OpenRiscCPModifier Modifier = OpenRiscCP::no_modifier);

  const char *getSymbol() const { return S.c_str(); }

  int getExistingMachineCPValue(MachineConstantPool *CP,
                                Align Alignment) override;

  void addSelectionDAGCSEId(FoldingSetNodeID &ID) override;

  /// hasSameValue - Return true if this OpenRisc constpool value can share the
  /// same constantpool entry as another OpenRisc constpool value.
  bool hasSameValue(OpenRiscConstantPoolValue *ACPV) override;

  bool isPrivateLinkage() { return PrivateLinkage; }

  void print(raw_ostream &O) const override;

  static bool classof(const OpenRiscConstantPoolValue *ACPV) {
    return ACPV->isExtSymbol();
  }

  bool equals(const OpenRiscConstantPoolSymbol *A) const {
    return S == A->S && OpenRiscConstantPoolValue::equals(A);
  }
};

/// OpenRiscConstantPoolMBB - OpenRisc-specific constantpool value of a machine
/// basic block.
class OpenRiscConstantPoolMBB : public OpenRiscConstantPoolValue {
  const MachineBasicBlock *MBB; // Machine basic block.

  OpenRiscConstantPoolMBB(LLVMContext &C, const MachineBasicBlock *M,
                        unsigned ID);

public:
  static OpenRiscConstantPoolMBB *Create(LLVMContext &C,
                                       const MachineBasicBlock *M, unsigned ID);

  const MachineBasicBlock *getMBB() const { return MBB; }

  int getExistingMachineCPValue(MachineConstantPool *CP,
                                Align Alignment) override;

  void addSelectionDAGCSEId(FoldingSetNodeID &ID) override;

  /// hasSameValue - Return true if this OpenRisc constpool value can share the
  /// same constantpool entry as another OpenRisc constpool value.
  bool hasSameValue(OpenRiscConstantPoolValue *ACPV) override;

  void print(raw_ostream &O) const override;

  static bool classof(const OpenRiscConstantPoolValue *ACPV) {
    return ACPV->isMachineBasicBlock();
  }

  bool equals(const OpenRiscConstantPoolMBB *A) const {
    return MBB == A->MBB && OpenRiscConstantPoolValue::equals(A);
  }
};

/// OpenRiscConstantPoolJumpTable - OpenRisc-specific constantpool values for Jump
/// Table symbols.
class OpenRiscConstantPoolJumpTable : public OpenRiscConstantPoolValue {
  unsigned Idx; // Jump Table Index.

  OpenRiscConstantPoolJumpTable(LLVMContext &C, unsigned Idx);

public:
  static OpenRiscConstantPoolJumpTable *Create(LLVMContext &C, unsigned Idx);

  unsigned getIndex() const { return Idx; }

  int getExistingMachineCPValue(MachineConstantPool *CP,
                                Align Alignment) override;

  void addSelectionDAGCSEId(FoldingSetNodeID &ID) override;

  /// hasSameValue - Return true if this OpenRisc constpool value can share the
  /// same constantpool entry as another OpenRisc constpool value.
  bool hasSameValue(OpenRiscConstantPoolValue *ACPV) override;

  void print(raw_ostream &O) const override;

  static bool classof(const OpenRiscConstantPoolValue *ACPV) {
    return ACPV->isJumpTable();
  }

  bool equals(const OpenRiscConstantPoolJumpTable *A) const {
    return Idx == A->Idx && OpenRiscConstantPoolValue::equals(A);
  }
};

} // namespace llvm

#endif /* LLVM_LIB_TARGET_OPENRISC_OPENRISCCONSTANTPOOLVALUE_H */
