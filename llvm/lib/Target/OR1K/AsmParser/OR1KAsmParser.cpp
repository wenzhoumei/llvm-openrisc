#include "MCTargetDesc/OR1KMCAsmInfo.h"
#include "MCTargetDesc/OR1KMCTargetDesc.h"
#include "TargetInfo/OR1KTargetInfo.h"
#include "llvm/ADT/StringSwitch.h"
#include "llvm/MC/MCContext.h"
#include "llvm/MC/MCExpr.h"
#include "llvm/MC/MCInst.h"
#include "llvm/MC/MCInstrInfo.h"
#include "llvm/MC/MCParser/AsmLexer.h"
#include "llvm/MC/MCParser/MCParsedAsmOperand.h"
#include "llvm/MC/MCParser/MCTargetAsmParser.h"
#include "llvm/MC/MCStreamer.h"
#include "llvm/MC/MCSubtargetInfo.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/Compiler.h"

using namespace llvm;

#define DEBUG_TYPE "or1k-asm-parser"

namespace {

struct OR1KOperand;

class OR1KAsmParser : public MCTargetAsmParser {
  const MCRegisterInfo &MRI;

  SMLoc getLoc() const { return getParser().getTok().getLoc(); }

  bool matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                               OperandVector &Operands, MCStreamer &Out,
                               uint64_t &ErrorInfo,
                               bool MatchingInlineAsm) override;

  bool parseRegister(MCRegister &Reg, SMLoc &StartLoc, SMLoc &EndLoc) override;
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;
  bool parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  ParseStatus parseDirective(AsmToken DirectiveID) override {
    return ParseStatus::NoMatch;
  }

#define GET_ASSEMBLER_HEADER
#include "OR1KGenAsmMatcher.inc"
  bool parseOperand(OperandVector &Operands, StringRef Mnemonic);

  ParseStatus parsePCRelTarget(OperandVector &Operands);
  ParseStatus parseMemDispBase(OperandVector &Operands);

  ParseStatus parseImmediate(OperandVector &Operands);
  ParseStatus parseRegister(OperandVector &Operands);

public:
  enum OR1KMatchResultTy {
    Match_Dummy = FIRST_TARGET_MATCH_RESULT_TY,
#define GET_OPERAND_DIAGNOSTIC_TYPES
#include "OR1KGenAsmMatcher.inc"
#undef GET_OPERAND_DIAGNOSTIC_TYPES
  };
  OR1KAsmParser(const MCSubtargetInfo &STI, MCAsmParser &Parser,
                const MCInstrInfo &MII, const MCTargetOptions &Options)
      : MCTargetAsmParser(Options, STI, MII),
        MRI(*Parser.getContext().getRegisterInfo()) {
    setAvailableFeatures(ComputeAvailableFeatures(STI.getFeatureBits()));
  }
};

struct OR1KOperand : public MCParsedAsmOperand {
  enum KindTy { Token, Register, Immediate } Kind;

  struct RegOp {
    MCRegister RegNum;
  };
  struct ImmOp {
    const MCExpr *Val;
  };

  SMLoc StartLoc, EndLoc;
  union {
    StringRef Tok;
    RegOp Reg;
    ImmOp Imm;
  };

  OR1KOperand(KindTy K) : Kind(K) {}

  bool isSImm16() const {
    if (!isImm())
      return false;
    if (auto *CE = dyn_cast<MCConstantExpr>(getImm())) {
      int64_t Val = CE->getValue();
      return isInt<16>(Val);
    }
    return false;
  }

  bool isUImm6() const {
    if (!isImm())
      return false;
    if (auto *CE = dyn_cast<MCConstantExpr>(getImm())) {
      int64_t Val = CE->getValue();
      return isUInt<6>(Val);
    }
    return false;
  }

  bool isUImm16() const {
    if (!isImm())
      return false;
    if (auto *CE = dyn_cast<MCConstantExpr>(getImm())) {
      int64_t Val = CE->getValue();
      return isUInt<16>(Val);
    }
    return false;
  }

  static std::unique_ptr<OR1KOperand> createToken(StringRef Str, SMLoc S) {
    auto Op = std::make_unique<OR1KOperand>(Token);
    Op->Tok = Str;
    Op->StartLoc = S;
    Op->EndLoc = S;
    return Op;
  }

  static std::unique_ptr<OR1KOperand> createReg(MCRegister R, SMLoc S,
                                                SMLoc E) {
    auto Op = std::make_unique<OR1KOperand>(Register);
    Op->Reg.RegNum = R;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  static std::unique_ptr<OR1KOperand> createImm(const MCExpr *V, SMLoc S,
                                                SMLoc E) {
    auto Op = std::make_unique<OR1KOperand>(Immediate);
    Op->Imm.Val = V;
    Op->StartLoc = S;
    Op->EndLoc = E;
    return Op;
  }

  // MCParsedAsmOperand API
  bool isToken() const override { return Kind == Token; }
  bool isReg() const override { return Kind == Register; }
  bool isImm() const override { return Kind == Immediate; }
  bool isMem() const override { return false; }

  SMLoc getStartLoc() const override { return StartLoc; }
  SMLoc getEndLoc() const override { return EndLoc; }

  MCRegister getReg() const override {
    assert(Kind == Register && "Invalid access!");
    return Reg.RegNum;
  }

  const MCExpr *getImm() const {
    assert(Kind == Immediate && "Invalid access!");
    return Imm.Val;
  }

  StringRef getToken() const {
    assert(Kind == Token && "Invalid access!");
    return Tok;
  }

  void print(raw_ostream &OS, const MCAsmInfo &MAI) const override {
    switch (Kind) {
    case Immediate:
      MAI.printExpr(OS, *getImm());
      break;
    case Register:
      OS << "<reg " << getReg() << ">";
      break;
    case Token:
      OS << '\'' << getToken() << '\'';
      break;
    }
  }

  // TableGen hook helpers
  void addRegOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    Inst.addOperand(MCOperand::createReg(getReg()));
  }
  void addImmOperands(MCInst &Inst, unsigned N) const {
    assert(N == 1 && "Invalid number of operands!");
    const MCExpr *E = getImm();
    if (auto *CE = dyn_cast<MCConstantExpr>(E))
      Inst.addOperand(MCOperand::createImm(CE->getValue()));
    else
      Inst.addOperand(MCOperand::createExpr(E));
  }
};
} // end anonymous namespace

// Auto-generated matchers
#define GET_REGISTER_MATCHER
#define GET_MATCHER_IMPLEMENTATION
#include "OR1KGenAsmMatcher.inc"

bool OR1KAsmParser::matchAndEmitInstruction(SMLoc IDLoc, unsigned &Opcode,
                                            OperandVector &Operands,
                                            MCStreamer &Out,
                                            uint64_t &ErrorInfo,
                                            bool MatchingInlineAsm) {
  MCInst Inst;
  auto Result =
      MatchInstructionImpl(Operands, Inst, ErrorInfo, MatchingInlineAsm);

  switch (Result) {
  default:
    break;
  case Match_Success:
    Inst.setLoc(IDLoc);
    Out.emitInstruction(Inst, getSTI());
    return false;
  case Match_MissingFeature:
    return Error(IDLoc, "instruction requires an enabled feature");
  case Match_MnemonicFail:
    return Error(IDLoc, "unrecognized instruction mnemonic");
  case Match_InvalidOperand: {
    SMLoc ErrorLoc = IDLoc;
    if (ErrorInfo != ~0U) {
      if (ErrorInfo >= Operands.size())
        return Error(ErrorLoc, "too few operands for instruction");

      ErrorLoc = ((OR1KOperand &)*Operands[ErrorInfo]).getStartLoc();
      if (ErrorLoc == SMLoc())
        ErrorLoc = IDLoc;
    }
    return Error(ErrorLoc, "invalid operand for instruction");
  }
  }

  report_fatal_error("Unknown match type detected!");
}

bool OR1KAsmParser::parseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                  SMLoc &EndLoc) {
  if (!tryParseRegister(Reg, StartLoc, EndLoc).isSuccess())
    return Error(StartLoc, "invalid register name");
  return false;
}

ParseStatus OR1KAsmParser::tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                                            SMLoc &EndLoc) {
  const AsmToken &Tok = getParser().getTok();
  StartLoc = Tok.getLoc();
  EndLoc = Tok.getEndLoc();

  if (Tok.isNot(AsmToken::Identifier))
    return ParseStatus::NoMatch;

  StringRef Name = getLexer().getTok().getIdentifier();
  Reg = MatchRegisterName(Name);
  if (!Reg)
    Reg = MatchRegisterAltName(Name);
  if (!Reg)
    return ParseStatus::NoMatch;
  getParser().Lex(); // Eat identifier token.
  return ParseStatus::Success;
}

bool OR1KAsmParser::parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                     SMLoc NameLoc, OperandVector &Operands) {
  // First operand is token for instruction
  Operands.push_back(OR1KOperand::createToken(Name, NameLoc));

  // If there are no more operands, then finish
  if (getLexer().is(AsmToken::EndOfStatement)) {
    getParser().Lex(); // Consume the EndOfStatement.
    return false;
  }

  // Parse first operand
  if (parseOperand(Operands, Name))
    return true;

  // Parse until end of statement, consuming commas between operands
  while (parseOptionalToken(AsmToken::Comma)) {
    if (parseOperand(Operands, Name))
      return true;
  }

  if (getParser().parseEOL("unexpected token")) {
    getParser().eatToEndOfStatement();
    return true;
  }
  return false;
}


bool OR1KAsmParser::parseOperand(OperandVector &Operands, StringRef Mnemonic) {
  // Check if current operand has custom associated parser.
  ParseStatus Res = MatchOperandParserImpl(Operands, Mnemonic);
  if (Res.isSuccess())
    return false;
  if (Res.isFailure())
    return true;

  // Fallback to the general approach.
  if (parseRegister(Operands).isSuccess())
    return false;
  if (parseImmediate(Operands).isSuccess())
    return false;

  return Error(getLoc(), "unknown operand");
}

ParseStatus OR1KAsmParser::parseMemDispBase(OperandVector &Operands) {
  MCAsmParser &P = getParser();
  AsmLexer  &L = getLexer();

  SMLoc Start = L.getLoc();

  // Form A: (reg)  ==>  0(reg)
  if (L.is(AsmToken::LParen)) {
    P.Lex(); // '('
    MCRegister Base; SMLoc RS, RE;
    auto S = tryParseRegister(Base, RS, RE);
    if (!S.isSuccess()) {
      Error(Start, "expected register after '('");
      return ParseStatus::Failure;
    }
    if (!parseOptionalToken(AsmToken::RParen)) {
      Error(Start, "expected ')'");
      return ParseStatus::Failure;
    }
    const MCExpr *Zero = MCConstantExpr::create(0, getContext());
    Operands.push_back(OR1KOperand::createImm(Zero, Start, L.getLoc()));
    Operands.push_back(OR1KOperand::createReg(Base, RS, RE));
    return ParseStatus::Success;
  }

  // Form B: expr or expr(reg)
  // Only try if the token can start an expression.
  switch (L.getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::Minus:
  case AsmToken::Plus:
  case AsmToken::Tilde:
  case AsmToken::Exclaim:
  case AsmToken::Integer:
  case AsmToken::Identifier:
  case AsmToken::String:
  case AsmToken::Dot:
    break;
  }

  const MCExpr *Disp = nullptr; SMLoc End;
  if (P.parseExpression(Disp, End))
    return ParseStatus::Failure;

  // Optional "(reg)" after the displacement
  if (parseOptionalToken(AsmToken::LParen)) {
    MCRegister Base; SMLoc RS, RE;
    auto S = tryParseRegister(Base, RS, RE);
    if (!S.isSuccess()) {
      Error(Start, "expected register after '('");
      return ParseStatus::Failure;
    }
    if (!parseOptionalToken(AsmToken::RParen)) {
      Error(Start, "expected ')'");
      return ParseStatus::Failure;
    }
    Operands.push_back(OR1KOperand::createImm(Disp, Start, End));
    Operands.push_back(OR1KOperand::createReg(Base, RS, RE));
    return ParseStatus::Success;
  }

  // No base register: treat as a plain immediate (useful for other operands).
  Operands.push_back(OR1KOperand::createImm(Disp, Start, End));
  return ParseStatus::Success;
}

ParseStatus OR1KAsmParser::parsePCRelTarget(OperandVector &Operands) {
  const MCExpr *Expr = nullptr;
  SMLoc S = getLoc(), E;
  if (getParser().parseExpression(Expr, E))
    return ParseStatus::Failure;

  Operands.push_back(OR1KOperand::createImm(Expr, S, E));
  return ParseStatus::Success;
}

ParseStatus OR1KAsmParser::parseImmediate(OperandVector &Operands) {
  SMLoc S = getLoc();
  SMLoc E;
  const MCExpr *Res;

  switch (getLexer().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::LParen:
  case AsmToken::Minus:
  case AsmToken::Plus:
  case AsmToken::Tilde:
  case AsmToken::Integer:
  case AsmToken::String:
    if (getParser().parseExpression(Res, E))
      return ParseStatus::Failure;
    break;
  }

  Operands.push_back(OR1KOperand::createImm(Res, S, E));
  return ParseStatus::Success;
}

ParseStatus OR1KAsmParser::parseRegister(OperandVector &Operands) {
  MCRegister R; SMLoc S, E;
  auto St = tryParseRegister(R, S, E);
  if (St.isNoMatch())  return ParseStatus::NoMatch;
  if (St.isFailure())  return ParseStatus::Failure;

  Operands.push_back(OR1KOperand::createReg(R, S, E));
  return ParseStatus::Success;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOR1KAsmParser() {
  RegisterMCAsmParser<OR1KAsmParser> X(getTheOR1KTarget());
}
