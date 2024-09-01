//===- OpenRiscCallingConv.td - OpenRisc Calling Conventions -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===-------------------------------------------------------------------------===//

def CC_OpenRisc : CallingConv<[
  // Promote small types to i32.
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  
  // Assign i32 arguments to registers R3 through R8.
  CCIfType<[i32], CCAssignToReg<[R3,R4,R5,R6,R7,R8]>>,

  // Assign remaining arguments to the stack, with 4-byte alignment.
  CCAssignToStack<4, 4>
]>;

def RetCC_OpenRisc : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[R11]>>,
]>;

def CSR : CalleeSavedRegs<(add R14,R16,R18,R20,R22,R24,R26,R28,R30)>;