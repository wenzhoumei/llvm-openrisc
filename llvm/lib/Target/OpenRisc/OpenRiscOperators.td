//===- OpenRiscOperators.td - OpenRisc-specific operators ---------*- tblgen-*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Type profiles
//===----------------------------------------------------------------------===//
def SDT_OpenRiscCallSeqStart        : SDCallSeqStart<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_OpenRiscCallSeqEnd          : SDCallSeqEnd<[SDTCisVT<0, i32>, SDTCisVT<1, i32>]>;
def SDT_OpenRiscCall                : SDTypeProfile<0, -1, [SDTCisPtrTy<0>]>;

def SDT_OpenRiscWrapPtr             : SDTypeProfile<1, 1,
                                                 [SDTCisSameAs<0, 1>,
                                                  SDTCisPtrTy<0>]>;

def SDT_OpenRiscBrJT                : SDTypeProfile<0, 2,
                                                 [SDTCisPtrTy<0>, SDTCisVT<1, i32>]>;

def SDT_OpenRiscSelectCC            : SDTypeProfile<1, 5,
                                                 [SDTCisSameAs<0, 1>,
                                                  SDTCisSameAs<2, 3>,
                                                  SDTCisVT<5, i32>]>;
//===----------------------------------------------------------------------===//
// Node definitions
//===----------------------------------------------------------------------===//
def OpenRisc_call: SDNode<"OpenRiscISD::CALL", SDT_OpenRiscCall,
                       [SDNPHasChain, SDNPOutGlue, SDNPOptInGlue, SDNPVariadic]>;

def OpenRisc_ret: SDNode<"OpenRiscISD::RET", SDTNone,
                       [SDNPHasChain, SDNPOptInGlue, SDNPVariadic]>;

def OpenRisc_pcrel_wrapper: SDNode<"OpenRiscISD::PCREL_WRAPPER", SDT_OpenRiscWrapPtr, []>;

def OpenRisc_callseq_start: SDNode<"ISD::CALLSEQ_START", SDT_OpenRiscCallSeqStart,
                                [SDNPHasChain, SDNPSideEffect, SDNPOutGlue]>;

def OpenRisc_callseq_end  : SDNode<"ISD::CALLSEQ_END",   SDT_OpenRiscCallSeqEnd,
                                [SDNPHasChain, SDNPSideEffect, SDNPOptInGlue,
                                 SDNPOutGlue]>;

def OpenRisc_select_cc: SDNode<"OpenRiscISD::SELECT_CC", SDT_OpenRiscSelectCC,
                            [SDNPInGlue]>;
