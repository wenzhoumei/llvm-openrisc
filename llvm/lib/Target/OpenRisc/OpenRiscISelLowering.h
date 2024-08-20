//===- OpenRiscISelLowering.h - OpenRisc DAG Lowering Interface -----*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file defines the interfaces that OpenRisc uses to lower LLVM code into a
// selection DAG.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_OPENRISCISELLOWERING_H
#define LLVM_LIB_TARGET_OPENRISC_OPENRISCISELLOWERING_H

#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/SelectionDAG.h"
#include "llvm/CodeGen/TargetLowering.h"

namespace llvm {

namespace OpenRiscISD {
enum {
  FIRST_NUMBER = ISD::BUILTIN_OP_END,
  BR_JT,

  // Calls a function.  Operand 0 is the chain operand and operand 1
  // is the target address.  The arguments start at operand 2.
  // There is an optional glue operand at the end.
  CALL,

  // Wraps a TargetGlobalAddress that should be loaded using PC-relative
  // accesses.  Operand 0 is the address.
  PCREL_WRAPPER,
  RET,

  // Select with condition operator - This selects between a true value and
  // a false value (ops #2 and #3) based on the boolean result of comparing
  // the lhs and rhs (ops #0 and #1) of a conditional expression with the
  // condition code in op #4
  SELECT_CC,
};
}

class OpenRiscSubtarget;

class OpenRiscTargetLowering : public TargetLowering {
public:
  explicit OpenRiscTargetLowering(const TargetMachine &TM,
                                const OpenRiscSubtarget &STI);

  EVT getSetCCResultType(const DataLayout &, LLVMContext &,
                         EVT VT) const override {
    if (!VT.isVector())
      return MVT::i32;
    return VT.changeVectorElementTypeToInteger();
  }

  bool isOffsetFoldingLegal(const GlobalAddressSDNode *GA) const override;

  const char *getTargetNodeName(unsigned Opcode) const override;

  SDValue LowerOperation(SDValue Op, SelectionDAG &DAG) const override;

  SDValue LowerFormalArguments(SDValue Chain, CallingConv::ID CallConv,
                               bool isVarArg,
                               const SmallVectorImpl<ISD::InputArg> &Ins,
                               const SDLoc &DL, SelectionDAG &DAG,
                               SmallVectorImpl<SDValue> &InVals) const override;

  SDValue LowerCall(CallLoweringInfo &CLI,
                    SmallVectorImpl<SDValue> &InVals) const override;

  bool CanLowerReturn(CallingConv::ID CallConv, MachineFunction &MF,
                      bool isVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      LLVMContext &Context) const override;

  SDValue LowerReturn(SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
                      const SmallVectorImpl<ISD::OutputArg> &Outs,
                      const SmallVectorImpl<SDValue> &OutVals, const SDLoc &DL,
                      SelectionDAG &DAG) const override;

  const OpenRiscSubtarget &getSubtarget() const { return Subtarget; }

  MachineBasicBlock *
  EmitInstrWithCustomInserter(MachineInstr &MI,
                              MachineBasicBlock *BB) const override;

private:
  const OpenRiscSubtarget &Subtarget;

  SDValue LowerBR_JT(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerImmediate(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerBlockAddress(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerJumpTable(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerConstantPool(ConstantPoolSDNode *CP, SelectionDAG &DAG) const;

  SDValue LowerSELECT_CC(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerDYNAMIC_STACKALLOC(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerSTACKSAVE(SDValue Op, SelectionDAG &DAG) const;

  SDValue LowerSTACKRESTORE(SDValue Op, SelectionDAG &DAG) const;

  SDValue getAddrPCRel(SDValue Op, SelectionDAG &DAG) const;

  CCAssignFn *CCAssignFnForCall(CallingConv::ID CC, bool IsVarArg) const;

  MachineBasicBlock *emitSelectCC(MachineInstr &MI,
                                  MachineBasicBlock *BB) const;
};

} // end namespace llvm

#endif /* LLVM_LIB_TARGET_OPENRISC_OPENRISCISELLOWERING_H */
