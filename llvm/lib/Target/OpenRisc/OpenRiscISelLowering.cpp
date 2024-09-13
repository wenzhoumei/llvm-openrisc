//===- OpenRiscISelLowering.cpp - OpenRisc DAG Lowering Implementation --------===//
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

#include "OpenRiscISelLowering.h"
#include "OpenRiscSubtarget.h"
#include "OpenRiscTargetMachine.h"
#include "llvm/CodeGen/CallingConvLower.h"
#include "llvm/CodeGen/MachineFrameInfo.h"
#include "llvm/CodeGen/MachineInstrBuilder.h"
#include "llvm/CodeGen/MachineJumpTableInfo.h"
#include "llvm/CodeGen/MachineRegisterInfo.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/ErrorHandling.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/raw_ostream.h"
#include <deque>

using namespace llvm;

#define DEBUG_TYPE "openrisc-lower"

// Return true if we must use long (in fact, indirect) function call.
// It's simplified version, production implimentation must
// resolve a functions in ROM (usually glibc functions)
static bool isLongCall(const char *str) {
  // Currently always use long calls
  return true;
}

OpenRiscTargetLowering::OpenRiscTargetLowering(const TargetMachine &TM,
                                               const OpenRiscSubtarget &STI)
    : TargetLowering(TM), Subtarget(STI) {
  MVT PtrVT = MVT::i32;
  
  // Set up the registers
  addRegisterClass(MVT::i32, &OpenRisc::GPRRegClass);

  setStackPointerRegisterToSaveRestore(OpenRisc::R1);

  setSchedulingPreference(Sched::RegPressure);
  setMinFunctionAlignment(Align(4));

  setBooleanContents(ZeroOrOneBooleanContent);

  // Specify [Legal|Expand|Promote|Custom] for each SDNode
  setOperationAction(ISD::Constant, MVT::i32, Legal);
  setOperationAction(ISD::Constant, MVT::i64, Expand);

  setOperationAction(ISD::GlobalAddress, PtrVT, Custom);

  setOperationAction(ISD::MUL, MVT::i32, Expand);
  setOperationAction(ISD::SMUL_LOHI, MVT::i32, Expand);
  setOperationAction(ISD::UMUL_LOHI, MVT::i32, Expand);

  setOperationAction(ISD::SDIV, MVT::i32, Expand);
  setOperationAction(ISD::SDIVREM, MVT::i32, Expand);
  setOperationAction(ISD::UDIV, MVT::i32, Expand);
  setOperationAction(ISD::UDIVREM, MVT::i32, Expand);

  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i1, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i8, Expand);
  setOperationAction(ISD::SIGN_EXTEND_INREG, MVT::i16, Expand);

  setOperationAction(ISD::SETCC, MVT::i32, Expand);
  setOperationAction(ISD::SELECT_CC, MVT::i32, Custom);


  // Compute derived properties from the register classes
  computeRegisterProperties(STI.getRegisterInfo());
}

//===----------------------------------------------------------------------===//
// Calling conventions
//===----------------------------------------------------------------------===//

#include "OpenRiscGenCallingConv.inc"

SDValue OpenRiscTargetLowering::LowerFormalArguments(
    SDValue Chain, CallingConv::ID CallConv, bool IsVarArg,
    const SmallVectorImpl<ISD::InputArg> &Ins, const SDLoc &DL,
    SelectionDAG &DAG, SmallVectorImpl<SDValue> &InVals) const {
  MachineFunction &MF = DAG.getMachineFunction();
  MachineFrameInfo &MFI = MF.getFrameInfo();

  // Used with vargs to acumulate store chains.
  std::vector<SDValue> OutChains;

  if (IsVarArg)
    report_fatal_error("Var arg not supported by FormalArguments Lowering");

  // Assign locations to all of the incoming arguments.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, DAG.getMachineFunction(), ArgLocs,
                 *DAG.getContext());

  CCInfo.AnalyzeFormalArguments(Ins, CC_OpenRisc);

  for (unsigned i = 0, e = ArgLocs.size(); i != e; ++i) {
    CCValAssign &VA = ArgLocs[i];
    
    if (VA.isRegLoc()) {
      // Arguments passed in registers
      EVT RegVT = VA.getLocVT();
      const TargetRegisterClass *RC;

      if (RegVT == MVT::i32)
        RC = &OpenRisc::GPRRegClass;
      else
        report_fatal_error("RegVT not supported by FormalArguments Lowering");

      // Transform the arguments stored on
      // physical registers into virtual ones
      unsigned Register = MF.addLiveIn(VA.getLocReg(), RC);
      SDValue ArgValue = DAG.getCopyFromReg(Chain, DL, Register, RegVT);

      // If this is an 8 or 16-bit value, it has been passed promoted
      // to 32 bits.  Insert an assert[sz]ext to capture this, then
      // truncate to the right size.
      if (VA.getLocInfo() != CCValAssign::Full) {
        unsigned Opcode = 0;
        if (VA.getLocInfo() == CCValAssign::SExt)
          Opcode = ISD::AssertSext;
        else if (VA.getLocInfo() == CCValAssign::ZExt)
          Opcode = ISD::AssertZext;
        if (Opcode)
          ArgValue = DAG.getNode(Opcode, DL, RegVT, ArgValue,
                                 DAG.getValueType(VA.getValVT()));
        ArgValue = DAG.getNode((VA.getValVT() == MVT::f32) ? ISD::BITCAST
                                                           : ISD::TRUNCATE,
                               DL, VA.getValVT(), ArgValue);
      }

      InVals.push_back(ArgValue);

    } else {
      assert(VA.isMemLoc());

      EVT ValVT = VA.getValVT();

      // The stack pointer offset is relative to the caller stack frame.
      int FI = MFI.CreateFixedObject(ValVT.getStoreSize(), VA.getLocMemOffset(),
                                     true);

      if (Ins[VA.getValNo()].Flags.isByVal()) {
        // Assume that in this case load operation is created
        SDValue FIN = DAG.getFrameIndex(FI, MVT::i32);
        InVals.push_back(FIN);
      } else {
        // Create load nodes to retrieve arguments from the stack
        SDValue FIN =
            DAG.getFrameIndex(FI, getFrameIndexTy(DAG.getDataLayout()));
        InVals.push_back(DAG.getLoad(
            ValVT, DL, Chain, FIN,
            MachinePointerInfo::getFixedStack(DAG.getMachineFunction(), FI)));
      }
    }
  }

  // All stores are grouped in one node to allow the matching between
  // the size of Ins and InVals. This only happens when on varg functions
  if (!OutChains.empty()) {
    OutChains.push_back(Chain);
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, OutChains);
  }

  return Chain;
}

SDValue
OpenRiscTargetLowering::LowerCall(CallLoweringInfo &CLI,
                                SmallVectorImpl<SDValue> &InVals) const {
  SelectionDAG &DAG = CLI.DAG;
  SDLoc &DL = CLI.DL;
  SmallVector<ISD::OutputArg, 32> &Outs = CLI.Outs;
  SmallVector<SDValue, 32> &OutVals = CLI.OutVals;
  SmallVector<ISD::InputArg, 32> &Ins = CLI.Ins;
  SDValue Chain = CLI.Chain;
  SDValue Callee = CLI.Callee;
  bool &IsTailCall = CLI.IsTailCall;
  CallingConv::ID CallConv = CLI.CallConv;
  bool IsVarArg = CLI.IsVarArg;

  MachineFunction &MF = DAG.getMachineFunction();
  EVT PtrVT = getPointerTy(DAG.getDataLayout());
  const TargetFrameLowering *TFL = Subtarget.getFrameLowering();

  // TODO: Support tail call optimization.
  IsTailCall = false;

  // Analyze the operands of the call, assigning locations to each operand.
  SmallVector<CCValAssign, 16> ArgLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, ArgLocs, *DAG.getContext());

  CCInfo.AnalyzeCallOperands(Outs, CC_OpenRisc);

  // Get a count of how many bytes are to be pushed on the stack.
  unsigned NumBytes = CCInfo.getStackSize();

  Align StackAlignment = TFL->getStackAlign();
  unsigned NextStackOffset = alignTo(NumBytes, StackAlignment);

  Chain = DAG.getCALLSEQ_START(Chain, NextStackOffset, 0, DL);

  // Copy argument values to their designated locations.
  std::deque<std::pair<unsigned, SDValue>> RegsToPass;
  SmallVector<SDValue, 8> MemOpChains;
  SDValue StackPtr;
  for (unsigned I = 0, E = ArgLocs.size(); I != E; ++I) {
    CCValAssign &VA = ArgLocs[I];
    SDValue ArgValue = OutVals[I];
    ISD::ArgFlagsTy Flags = Outs[I].Flags;

    if (VA.isRegLoc())
      // Queue up the argument copies and emit them at the end.
      RegsToPass.push_back(std::make_pair(VA.getLocReg(), ArgValue));
    else if (Flags.isByVal()) {
      assert(VA.isMemLoc());
      assert(Flags.getByValSize() &&
             "ByVal args of size 0 should have been ignored by front-end.");
      assert(!IsTailCall &&
             "Do not tail-call optimize if there is a byval argument.");

      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, DL, OpenRisc::R1, PtrVT);
      unsigned Offset = VA.getLocMemOffset();
      SDValue Address = DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr,
                                    DAG.getIntPtrConstant(Offset, DL));
      SDValue SizeNode = DAG.getConstant(Flags.getByValSize(), DL, MVT::i32);
      SDValue Memcpy = DAG.getMemcpy(
          Chain, DL, Address, ArgValue, SizeNode, Flags.getNonZeroByValAlign(),
          /*isVolatile=*/false, /*AlwaysInline=*/false,
          /*CI=*/nullptr, std::nullopt, MachinePointerInfo(), MachinePointerInfo());
      MemOpChains.push_back(Memcpy);
    } else {
      assert(VA.isMemLoc() && "Argument not register or memory");

      // Work out the address of the stack slot.  Unpromoted ints and
      // floats are passed as right-justified 8-byte values.
      if (!StackPtr.getNode())
        StackPtr = DAG.getCopyFromReg(Chain, DL, OpenRisc::R1, PtrVT);
      unsigned Offset = VA.getLocMemOffset();
      SDValue Address = DAG.getNode(ISD::ADD, DL, PtrVT, StackPtr,
                                    DAG.getIntPtrConstant(Offset, DL));

      // Emit the store.
      MemOpChains.push_back(
          DAG.getStore(Chain, DL, ArgValue, Address, MachinePointerInfo()));
    }
  }

  // Join the stores, which are independent of one another.
  if (!MemOpChains.empty())
    Chain = DAG.getNode(ISD::TokenFactor, DL, MVT::Other, MemOpChains);

  // Build a sequence of copy-to-reg nodes, chained and glued together.
  SDValue Glue;
  for (unsigned I = 0, E = RegsToPass.size(); I != E; ++I) {
    unsigned Reg = RegsToPass[I].first;
    Chain = DAG.getCopyToReg(Chain, DL, Reg, RegsToPass[I].second, Glue);
    Glue = Chain.getValue(1);
  }
  std::string name;
  unsigned char TF = 0;

  // Accept direct calls by converting symbolic call addresses to the
  // associated Target* opcodes.
  if (ExternalSymbolSDNode *E = dyn_cast<ExternalSymbolSDNode>(Callee)) {
    name = E->getSymbol();
    TF = E->getTargetFlags();
    if (isPositionIndependent()) {
      report_fatal_error("PIC relocations is not supported");
    } else
      Callee = DAG.getTargetExternalSymbol(E->getSymbol(), PtrVT, TF);
  } else if (GlobalAddressSDNode *G = dyn_cast<GlobalAddressSDNode>(Callee)) {
    const GlobalValue *GV = G->getGlobal();
    name = GV->getName().str();
  }

  if ((!name.empty()) && isLongCall(name.c_str())) {
    // TODO: Create a constant pool entry for the callee address
  }

  // The first call operand is the chain and the second is the target address.
  SmallVector<SDValue, 8> Ops;
  Ops.push_back(Chain);
  Ops.push_back(Callee);

  // Add a register mask operand representing the call-preserved registers.
  const TargetRegisterInfo *TRI = Subtarget.getRegisterInfo();
  const uint32_t *Mask = TRI->getCallPreservedMask(MF, CallConv);
  assert(Mask && "Missing call preserved mask for calling convention");
  Ops.push_back(DAG.getRegisterMask(Mask));

  // Add argument registers to the end of the list so that they are
  // known live into the call.
  for (unsigned I = 0, E = RegsToPass.size(); I != E; ++I) {
    unsigned Reg = RegsToPass[I].first;
    Ops.push_back(DAG.getRegister(Reg, RegsToPass[I].second.getValueType()));
  }

  // Glue the call to the argument copies, if any.
  if (Glue.getNode())
    Ops.push_back(Glue);

  SDVTList NodeTys = DAG.getVTList(MVT::Other, MVT::Glue);
  Chain = DAG.getNode(OpenRiscISD::CALL, DL, NodeTys, Ops);
  Glue = Chain.getValue(1);

  // Mark the end of the call, which is glued to the call itself.
  Chain = DAG.getCALLSEQ_END(Chain, DAG.getConstant(NumBytes, DL, PtrVT, true),
                             DAG.getConstant(0, DL, PtrVT, true), Glue, DL);
  Glue = Chain.getValue(1);

  // Assign locations to each value returned by this call.
  SmallVector<CCValAssign, 16> RetLocs;
  CCState RetCCInfo(CallConv, IsVarArg, MF, RetLocs, *DAG.getContext());
  RetCCInfo.AnalyzeCallResult(Ins, RetCC_OpenRisc);

  // Copy all of the result registers out of their specified physreg.
  for (unsigned I = 0, E = RetLocs.size(); I != E; ++I) {
    CCValAssign &VA = RetLocs[I];

    // Copy the value out, gluing the copy to the end of the call sequence.
    unsigned Reg = VA.getLocReg();
    SDValue RetValue = DAG.getCopyFromReg(Chain, DL, Reg, VA.getLocVT(), Glue);
    Chain = RetValue.getValue(1);
    Glue = RetValue.getValue(2);

    InVals.push_back(RetValue);
  }
  return Chain;
}

bool OpenRiscTargetLowering::CanLowerReturn(
    CallingConv::ID CallConv, MachineFunction &MF, bool IsVarArg,
    const SmallVectorImpl<ISD::OutputArg> &Outs, LLVMContext &Context) const {
  SmallVector<CCValAssign, 16> RVLocs;
  CCState CCInfo(CallConv, IsVarArg, MF, RVLocs, Context);
  return CCInfo.CheckReturn(Outs, RetCC_OpenRisc);
}

SDValue
OpenRiscTargetLowering::LowerReturn(SDValue Chain, CallingConv::ID CallConv,
                                  bool IsVarArg,
                                  const SmallVectorImpl<ISD::OutputArg> &Outs,
                                  const SmallVectorImpl<SDValue> &OutVals,
                                  const SDLoc &DL, SelectionDAG &DAG) const {
  if (IsVarArg)
    report_fatal_error("VarArg not supported");

  MachineFunction &MF = DAG.getMachineFunction();

  // Assign locations to each returned value.
  SmallVector<CCValAssign, 16> RetLocs;
  CCState RetCCInfo(CallConv, IsVarArg, MF, RetLocs, *DAG.getContext());
  RetCCInfo.AnalyzeReturn(Outs, RetCC_OpenRisc);

  SDValue Glue;
  // Quick exit for void returns
  if (RetLocs.empty())
    return DAG.getNode(OpenRiscISD::RET, DL, MVT::Other, Chain);

  // Copy the result values into the output registers.
  SmallVector<SDValue, 4> RetOps;
  RetOps.push_back(Chain);
  for (unsigned I = 0, E = RetLocs.size(); I != E; ++I) {
    CCValAssign &VA = RetLocs[I];
    SDValue RetValue = OutVals[I];

    // Make the return register live on exit.
    assert(VA.isRegLoc() && "Can only return in registers!");

    // Chain and glue the copies together.
    unsigned Register = VA.getLocReg();
    Chain = DAG.getCopyToReg(Chain, DL, Register, RetValue, Glue);
    Glue = Chain.getValue(1);
    RetOps.push_back(DAG.getRegister(Register, VA.getLocVT()));
  }

  // Update chain and glue.
  RetOps[0] = Chain;
  if (Glue.getNode())
    RetOps.push_back(Glue);

  return DAG.getNode(OpenRiscISD::RET, DL, MVT::Other, RetOps);
}

SDValue OpenRiscTargetLowering::LowerOperation(SDValue Op, SelectionDAG &DAG) const {
  switch (Op.getOpcode()) {
  default:
    report_fatal_error("Unexpected node to lower");
  case ISD::GlobalAddress:
    return LowerGlobalAddress(Op, DAG);
  case ISD::SELECT_CC:
    return LowerSelectCC(Op, DAG);
  }
}

const char *OpenRiscTargetLowering::getTargetNodeName(unsigned Opcode) const {
  switch (Opcode) {
  case OpenRiscISD::BR_JT:
    return "OpenRiscISD::BR_JT";
  case OpenRiscISD::CALL:
    return "OpenRiscISD::CALL";
  case OpenRiscISD::PCREL_WRAPPER:
    return "OpenRiscISD::PCREL_WRAPPER";
  case OpenRiscISD::RET:
    return "OpenRiscISD::RET";
  case OpenRiscISD::SELECT_CC:
    return "OpenRiscISD::SELECT_CC";
  case OpenRiscISD::GA_WRAPPER:
    return "OpenRiscISD::GA_WRAPPER";
  }
  return nullptr;
}

SDValue OpenRiscTargetLowering::LowerGlobalAddress(SDValue Op, SelectionDAG &DAG) const {
  auto DL = DAG.getDataLayout();

  const GlobalValue *GV = cast<GlobalAddressSDNode>(Op)->getGlobal();
  int64_t Offset = cast<GlobalAddressSDNode>(Op)->getOffset();

  // Create the TargetGlobalAddress node, folding in the constant offset.
  SDValue Result =
      DAG.getTargetGlobalAddress(GV, SDLoc(Op), getPointerTy(DL), Offset);
  return DAG.getNode(OpenRiscISD::GA_WRAPPER, SDLoc(Op), getPointerTy(DL), Result);
}

SDValue OpenRiscTargetLowering::LowerSelectCC(SDValue Op, SelectionDAG &DAG) const {
  SDValue LHS = Op.getOperand(0);
  SDValue RHS = Op.getOperand(1);
  ISD::CondCode CC = cast<CondCodeSDNode>(Op.getOperand(4))->get();
  SDValue TrueVal = Op.getOperand(2);
  SDValue FalseVal = Op.getOperand(3);
  SDLoc dl(Op);
  unsigned OpenRiscCC = (unsigned) CC;

  return DAG.getNode(OpenRiscISD::SELECT_CC, dl, TrueVal.getValueType(), LHS, RHS, TrueVal, FalseVal,
                     DAG.getConstant(OpenRiscCC, dl, MVT::i32));
}

MachineBasicBlock *
OpenRiscTargetLowering::EmitInstrWithCustomInserter(MachineInstr &MI,
                                                 MachineBasicBlock *BB) const {
  switch (MI.getOpcode()) {
  default: llvm_unreachable("Unknown SELECT_CC!");
  case OpenRisc::SELECT_CC:
    return expandSelectCC(MI, BB);
  }
}

MachineBasicBlock *
OpenRiscTargetLowering::expandSelectCC(MachineInstr &MI, MachineBasicBlock *BB) const {
  const TargetInstrInfo &TII = *Subtarget.getInstrInfo();
  DebugLoc dl = MI.getDebugLoc();
  unsigned CC = MI.getOperand(5).getImm();

  // To "insert" a SELECT_CC instruction, we actually have to insert the
  // triangle control-flow pattern. The incoming instruction knows the
  // destination vreg to set, the condition code register to branch on, the
  // true/false values to select between, and the condition code for the branch.
  //
  // We produce the following control flow:
  //     ThisMBB
  //     |  \
  //     |  IfFalseMBB
  //     | /
  //    SinkMBB
  const BasicBlock *LLVM_BB = BB->getBasicBlock();
  MachineFunction::iterator It = ++BB->getIterator();

  MachineBasicBlock *ThisMBB = BB;
  MachineFunction *F = BB->getParent();
  MachineBasicBlock *IfFalseMBB = F->CreateMachineBasicBlock(LLVM_BB);
  MachineBasicBlock *SinkMBB = F->CreateMachineBasicBlock(LLVM_BB);
  F->insert(It, IfFalseMBB);
  F->insert(It, SinkMBB);

  // Transfer the remainder of ThisMBB and its successor edges to SinkMBB.
  SinkMBB->splice(SinkMBB->begin(), ThisMBB,
                  std::next(MachineBasicBlock::iterator(MI)), ThisMBB->end());
  SinkMBB->transferSuccessorsAndUpdatePHIs(ThisMBB);

  // Set the new successors for ThisMBB.
  ThisMBB->addSuccessor(IfFalseMBB);
  ThisMBB->addSuccessor(SinkMBB);

  unsigned CondSetOpcode;
  switch (CC) {
    case ISD::SETEQ:
      CondSetOpcode = OpenRisc::SFEQ; // Replace OpenRisc with your target's namespace
      break;
    case ISD::SETNE:
      CondSetOpcode = OpenRisc::SFNE;
      break;
    case ISD::SETUGT:
      CondSetOpcode = OpenRisc::SFGTU;
      break;
    case ISD::SETUGE:
      CondSetOpcode = OpenRisc::SFGEU;
      break;
    case ISD::SETULT:
      CondSetOpcode = OpenRisc::SFLTU;
      break;
    case ISD::SETULE:
      CondSetOpcode = OpenRisc::SFLEU;
      break;
    case ISD::SETGT:
      CondSetOpcode = OpenRisc::SFGTS;
      break;
    case ISD::SETGE:
      CondSetOpcode = OpenRisc::SFGES;
      break;
    case ISD::SETLT:
      CondSetOpcode = OpenRisc::SFLTS;
      break;
    case ISD::SETLE:
      CondSetOpcode = OpenRisc::SFLES;
      break;
    default:
      llvm_unreachable("Unsupported condition code");
  }
  
  BuildMI(ThisMBB, dl, TII.get(CondSetOpcode))
    .addReg(OpenRisc::SRReg)
    .addReg(MI.getOperand(1).getReg())
    .addReg(MI.getOperand(2).getReg());

  BuildMI(ThisMBB, dl, TII.get(OpenRisc::BF))
    .addReg(OpenRisc::SRReg)
    .addMBB(SinkMBB);

  // IfFalseMBB just falls through to SinkMBB.
  IfFalseMBB->addSuccessor(SinkMBB);

  // %Result = phi [ %TrueValue, ThisMBB ], [ %FalseValue, IfFalseMBB ]
  BuildMI(*SinkMBB, SinkMBB->begin(), dl, TII.get(OpenRisc::PHI),
          MI.getOperand(0).getReg())
      .addReg(MI.getOperand(3).getReg())
      .addMBB(ThisMBB)
      .addReg(MI.getOperand(4).getReg())
      .addMBB(IfFalseMBB);

  MI.eraseFromParent(); // The pseudo instruction is gone now.
  return SinkMBB;
}