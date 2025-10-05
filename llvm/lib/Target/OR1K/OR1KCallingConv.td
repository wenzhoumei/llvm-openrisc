//===- OR1KCallingConv.td - OR1K Calling Conventions -------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

def CC_OR1K : CallingConv<[
  CCIfType<[i1, i8, i16], CCPromoteToType<i32>>,
  CCIfType<[i32], CCAssignToReg<[R3,R4,R5,R6,R7,R8]>>,
  CCAssignToStack<4, 4>
]>;

def RetCC_OR1K : CallingConv<[
  CCIfType<[i32], CCAssignToReg<[R11, R12]>>,
]>;

def CSR_OR1K : CalleeSavedRegs<(add R14,R16,R18,R20,R22,R24,R26,R28,R30)>;
