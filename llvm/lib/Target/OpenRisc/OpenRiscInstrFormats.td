//===- OpenRiscInstrFormats.td - OpenRisc Instruction Formats -*- tablegen -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===--------------------------------------------------------------------------===//

 // TODO
 // - write classes for immeidates
 // - figure out for format 7 how to split imms
 // - figure out branchOp, setcc, etc

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
class F1_Inst<bits<6> opcode1, bits<2> opcode2, bits<4> opcode3, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
    : OpenRisc_Inst<(outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
                    opcodeAsm#" $rd, $ra, $rb",
                    [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))], itin> {
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
// Includes: l.addi, l.addic, l.andi, l.lbs, l.lbz, l.ld, l.lhs, l.lhz, l.lws, l.lwz, l.mfspr, l.mtspr, l.ori, xori
class F2_Inst<bits<6> opcode, dag outs, dag ins, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
    : OpenRisc_Inst<(outs GPR:$rd), (ins GPR:$ra, immOp:$imm),
                    opcodeAsm#" $rd, $ra, $imm", 
                    [(set GPR:$rd, (opNode GPR:$ra, immOp:$imm))], itin> {
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
class F3_Inst<bits<6> opcode1, bits<4> opcode2, bits<4> opcode3, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
    : OpenRisc_Inst<(outs GPR:$rd), (ins GPR:$ra, GPR:$rb),
                    opcodeAsm#" $rd, $ra, $rb", 
                    [(set GPR:$rd, (opNode GPR:$ra, GPR:$rb))], itin> {
  bits<5> rd;
  bits<5> ra;
  bits<5> rb;
  bits<1> reserved1;
  bits<2> reserved2;
  
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
class F4_Inst<bits<6> opcode1, bits<2> opcode2, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs GPR:$rd), (ins GPR:$ra, immOp:$imm),
                  opcodeAsm#" $rd, $ra, $imm", 
                  [(set GPR:$rd, (opNode GPR:$ra, immOp:$imm))], itin> {
  bits<5> rd;
  bits<5> ra;
  bits<8> reserved;
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
class F5_Inst<bits<6>  opcode, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs), (ins immOp:$imm),
                  opcodeAsm#" $imm", 
                  [(opNode immOp:$imm)], itin> {
  bits<26> imm;

  let Inst{31-26} = opcode;
  let Inst{25-0}  = imm;
}

//===----------------------------------------------------------------------===//
// Format 6
//===----------------------------------------------------------------------===//
// l.jalr, l.jr
class F6_Inst<bits<6> opcode, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs), (ins GPR:$rb),
            opcodeAsm#" $rb", 
            [(opNode GPR:$rb)], itin> {
  bits<10> reserved1;
  bits<5>  rb;
  bits<11> reserved2;

  let Inst{31-26} = opcode;
  let Inst{25-16} = reserved1;
  let Inst{15-11} = rb;
  let Inst{10-0}  = reserved2;
}

//===----------------------------------------------------------------------===//
// Format 7
//===----------------------------------------------------------------------===//
// Includes: l.sb, l.sd, l.sh, sw
class F7_Inst<bits<6> opcode, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs), (ins GPR:$ra, GPR:$rb, immOp:$imm),
                  opcodeAsm#" $imm($ra), $rb", 
                  [(opNode GPR:$ra, GPR:$rb, immOp:$imm)], itin> {
  bits<5>  imm1;
  bits<5>  ra;
  bits<5>  rb;
  bits<11> imm2;

  // TODO
  field imm1 = imm[4:0];   // Lower 5 bits
  field imm2 = imm[15:5];  // Upper 11 bits

  let Inst{31-26} = opcode;
  let Inst{25-21} = imm1;
  let Inst{20-16} = ra;
  let Inst{15-11} = rb;
  let Inst{10-0}  = imm2;
}

//===----------------------------------------------------------------------===//
// Format 8
//===----------------------------------------------------------------------===//
// Includes: l.sfeq, l.sfges, l.sfgeu, l.sfgts, l.sfgtu, l.sfles, l.sfleu, l.sflts, l.sfltu, l.sfne
class F8_Inst<bits<11> opcode, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs), (ins GPR:$ra, GPR:$rb),
                  opcodeAsm#" $ra, $rb", 
                  [(opNode GPR:$ra, GPR:$rb)], itin> {
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
class F9_Inst<bits<6> opcode1, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs GPR:$rd), (ins immOp:$imm),
            opcodeAsm#" $rd, $imm", 
            [(set GPR:$rd, (opNode immOp:$imm))], itin> {
  bits<5>  rd;
  bits<4>  reserved;
  bits<1>  opcode2;
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
class F10_Inst<bits<8> opcode, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs), (ins immOp:$imm),
                  opcodeAsm#" $rd, $imm",
                  [(set GPR:$rd, (opNode immOp:$imm))], itin> {
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
class F11_Inst<bits<6> opcode, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs), (ins immOp:$imm),
                  opcodeAsm,
                  [(opNode)], itin> {
  bits<26> reserved;

  let Inst{31-26} = opcode;
  let Inst{25-0}  = reserved;
}

//===----------------------------------------------------------------------===//
// Format 12
//===----------------------------------------------------------------------===//
// Includes: l.sys
class F12_Inst<bits<16> opcode, string opcodeAsm, SDPatternOperator opNode,
         InstrItinClass itin = NoItinerary>
  : OpenRisc_Inst<(outs), (ins immOp:$imm),
                  opcodeAsm#" $imm",
                  [(opNode immOp:$imm)], itin> {
  bits<16> imm;

  let Inst{31-16} = opcode;
  let Inst{15-0}  = imm;
}