//===- OpenRiscInstrFormats.td - OpenRisc Instruction Formats -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===--------------------------------------------------------------------------===//

// Base class for OpenRisc instructions
class OpenRisc_Inst<dag outs, dag ins, string asmStr, list<dag> pattern,
                    InstrItinClass itin = NoItinerary>
    : Instruction {
  field bits<32> Inst;

  let Namespace = "OpenRisc";
  let DecoderNamespace = "OpenRisc";

  let Size = 4;
  
  dag OutOperandList = outs;
  dag InOperandList = ins;

  let asmString = asmStr;
  let Pattern = pattern;
  let Itinerary = itin;
}

//===----------------------------------------------------------------------===//
// Format 1
//===----------------------------------------------------------------------===//
// Includes: l.add, l.addc, l.and, l.or, l.sub, l.xor
class F1_Inst<bits<6> opcode1, bits<2> opcode2, bits<4> opcode3,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                 InstrItinClass itin = NoItinerary>) {
  bits<5> rd;
  bits<5> ra;
  bits<5> rb;
  bits<1> reserved1 = 0;
  bits<4> reserved2 = 0;
  
  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10}    = reserved1;
  let Inst{9-8}   = opcode2;
  let Inst{7-4}   = reserved2;
  let Inst{3-0}   = opcode3;
}

//===----------------------------------------------------------------------===//
// Format 2
//===----------------------------------------------------------------------===//
// Includes: l.addi, l.addic, l.andi, l.ori, xori
class F2_Inst<bits<6> opcode,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<5>  rd;
  bits<5>  ra;
  bits<16> imm;

  let Inst{31-26} = opcode;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-0}  = imm;
}

//===----------------------------------------------------------------------===//
// Format 3
//===----------------------------------------------------------------------===//
// Includes: l.sll, l.sra, l.srl
class F3_Inst<bits<6> opcode1, bits<4> opcode2, bits<4> opcode3,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<5> rd;
  bits<5> ra;
  bits<5> rb;
  bits<1> reserved1 = 0;
  bits<2> reserved2 = 0;
  
  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10}    = reserved1;
  let Inst{9-6}   = opcode2;
  let Inst{5-4}   = reserved2;
  let Inst{3-0}   = opcode3;
}

//===----------------------------------------------------------------------===//
// Format 4
//===----------------------------------------------------------------------===//
// Includes: l.slli, l.srai, l.srli
class F4_Inst<bits<6> opcode1, bits<2> opcode2,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<5> rd;
  bits<5> ra;
  bits<8> reserved = 0;
  bits<6> imm;

  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-8}  = reserved;
  let Inst{7-6}   = opcode2;
  let Inst{5-0}   = imm;
}

//===----------------------------------------------------------------------===//
// Format 5
//===----------------------------------------------------------------------===//
// Includes: l.bf, l.bnf, l.j, l.jal
class F5_Inst<bits<6>  opcode,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<26> imm;

  let Inst{31-26} = opcode;
  let Inst{25-0}  = imm;
}

//===----------------------------------------------------------------------===//
// Format 6
//===----------------------------------------------------------------------===//
// l.jalr, l.jr
class F6_Inst<bits<6> opcode,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<10> reserved1 = 0;
  bits<5>  rb;
  bits<11> reserved2 = 0;

  let Inst{31-26} = opcode;
  let Inst{25-16} = reserved1;
  let Inst{15-11} = rb;
  let Inst{10-0}  = reserved2;
}

//===----------------------------------------------------------------------===//
// Format 7
//===----------------------------------------------------------------------===//
// Includes: l.sb, l.sd, l.sh, sw
class F7_Inst<bits<6> opcode,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<16>  imm;
  bits<5>  ra;
  bits<5>  rb;

  let Inst{31-26} = opcode;
  let Inst{25-21} = imm{15-11};
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10-0}  = imm{10-0};
}

//===----------------------------------------------------------------------===//
// Format 8
//===----------------------------------------------------------------------===//
// Includes: l.sfeq, l.sfges, l.sfgeu, l.sfgts, l.sfgtu, l.sfles, l.sfleu, l.sflts, l.sfltu, l.sfne
class F8_Inst<bits<11> opcode,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<5>  ra;
  bits<5>  rb;
  bits<11> reserved;

  let Inst{31-21} = opcode;
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10-0}  = reserved;
}

//===----------------------------------------------------------------------===//
// Format 9
//===----------------------------------------------------------------------===//
// Includes: l.movhi
class F9_Inst<bits<6> opcode1, bits<1> opcode2,
              dag outs, dag ins, string asmStr, list<dag> pattern,
              InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {

  bits<5>  rd;
  bits<4>  reserved;
  bits<16> imm;
 
  let Inst{31-26} = opcode1;
  let Inst{25-21} = rd;
  let Inst{20-17} = reserved;
  let Inst{16}    = opcode2;
  let Inst{15-0}  = imm;
}

//===----------------------------------------------------------------------===//
// Format 10
//===----------------------------------------------------------------------===//
// Includes: l.nop
class F10_Inst<bits<8> opcode,
               dag outs, dag ins, string asmStr, list<dag> pattern,
               InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {

  bits<8>  reserved;
  bits<16> imm;

  let Inst{31-24} = opcode;
  let Inst{23-16} = reserved;
  let Inst{15-0}  = imm;
}

//===----------------------------------------------------------------------===//
// Format 11
//===----------------------------------------------------------------------===//
// Includes: l.rfe
class F11_Inst<bits<6> opcode,
               dag outs, dag ins, string asmStr, list<dag> pattern,
               InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<26> reserved;

  let Inst{31-26} = opcode;
  let Inst{25-0}  = reserved;
}

//===----------------------------------------------------------------------===//
// Format 12
//===----------------------------------------------------------------------===//
// Includes: l.sys
class F12_Inst<bits<16> opcode,
               dag outs, dag ins, string asmStr, list<dag> pattern,
               InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {
  bits<16> imm;

  let Inst{31-16} = opcode;
  let Inst{15-0}  = imm;
}

//===----------------------------------------------------------------------===//
// Format 13
//===----------------------------------------------------------------------===//
// Includes: l.lbs, l.lbz, l.ld, l.lhs, l.lhz, l.lws, l.lwz
class F13_Inst<bits<6> opcode,
               dag outs, dag ins, string asmStr, list<dag> pattern,
               InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {

  bits<5>  rd;
  bits<5>  ra;
  bits<16> imm;

  let Inst{31-26} = opcode;
  let Inst{25-21} = rd;
  let Inst{20-16} = ra;
  let Inst{15-0}  = imm;
}

//===----------------------------------------------------------------------===//
// Format 14
//===----------------------------------------------------------------------===//
// Includes: l.mfspr, l.mtspr
class F14_Inst<bits<6> opcode,
               dag outs, dag ins, string asmStr, list<dag> pattern,
               InstrItinClass itin = NoItinerary> :
  OpenRisc_Inst<(outs, ins, asmStr, pattern,
                InstrItinClass itin = NoItinerary>) {

  bits<5>  ra;
  bits<5>  rb;
  bits<16>  imm;

  let Inst{31-26} = opcode;
  let Inst{25-21} = imm{15-11};
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10-0}  = imm{10-0};
}