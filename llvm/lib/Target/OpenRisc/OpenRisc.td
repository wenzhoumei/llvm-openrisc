//===- OpenRisc.td - Describe the OpenRisc Target Machine ------*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// Target-independent interfaces
//===----------------------------------------------------------------------===//

include "llvm/Target/Target.td"

//===----------------------------------------------------------------------===//
// Subtarget Features.
//===----------------------------------------------------------------------===//

//===----------------------------------------------------------------------===//
// OpenRisc supported processors.
//===----------------------------------------------------------------------===//
class Proc<string Name, list<SubtargetFeature> Features>
    : Processor<Name, NoItineraries, Features>;

def : Proc<"generic", []>;

//===----------------------------------------------------------------------===//
// Register File Description
//===----------------------------------------------------------------------===//

include "OpenRiscRegisterInfo.td"

//===----------------------------------------------------------------------===//
// Calling Convention Description
//===----------------------------------------------------------------------===//

include "OpenRiscCallingConv.td"

//===----------------------------------------------------------------------===//
// Instruction Descriptions
//===----------------------------------------------------------------------===//

include "OpenRiscInstrInfo.td"

def OpenRiscInstrInfo : InstrInfo;

//===----------------------------------------------------------------------===//
// Target Declaration
//===----------------------------------------------------------------------===//

def OpenRiscAsmParser : AsmParser {
  let ShouldEmitMatchRegisterAltName = 1;
}

def OpenRiscInstPrinter : AsmWriter {
  string AsmWriterClassName  = "InstPrinter";
}

def OpenRisc : Target {
  let InstructionSet = OpenRiscInstrInfo;
  let AssemblyWriters = [OpenRiscInstPrinter];
  let AssemblyParsers = [OpenRiscAsmParser];
}

