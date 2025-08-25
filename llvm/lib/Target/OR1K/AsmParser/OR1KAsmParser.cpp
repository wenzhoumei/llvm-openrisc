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
  unsigned validateTargetOperandClass(MCParsedAsmOperand &Op,
                                      unsigned Kind) override;

  bool parseRegister(MCRegister &Reg, SMLoc &StartLoc, SMLoc &EndLoc) override;
  ParseStatus tryParseRegister(MCRegister &Reg, SMLoc &StartLoc,
                               SMLoc &EndLoc) override;
  bool parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                        SMLoc NameLoc, OperandVector &Operands) override;
  ParseStatus parseDirective(AsmToken DirectiveID) override;

#define GET_ASSEMBLER_HEADER
#include "OR1KGenAsmMatcher.inc"
  ParseStatus parseImmediate(OperandVector &Operands);
  ParseStatus parseRegister(OperandVector &Operands);
  ParseStatus parsePCRelTarget(OperandVector &Operands);

  bool parseOperand(OperandVector &Operands, StringRef Mnemonic);

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

static SMLoc refineErrorLoc(const SMLoc Loc, const OperandVector &Operands,
                            uint64_t ErrorInfo) {
  if (ErrorInfo != ~0ULL && ErrorInfo < Operands.size()) {
    SMLoc E = Operands[ErrorInfo]->getStartLoc();
    if (E != SMLoc())
      return E;
  }
  return Loc;
}

unsigned OR1KAsmParser::validateTargetOperandClass(MCParsedAsmOperand &AsmOp,
                                                   unsigned Kind) {
  // TODO: add casts between classes if your TableGen operands need it, e.g.
  // interpreting an immediate literal as a register alias, etc.
  // For now, always fail to defer to default diagnostics in matcher.
  return Match_InvalidOperand;
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
  StringRef Name = getLexer().getTok().getIdentifier();

  Reg = MatchRegisterName(Name);
  if (!Reg)
    Reg = MatchRegisterAltName(Name);

  if (!Reg)
    return ParseStatus::NoMatch;

  getParser().Lex(); // Eat identifier token.
  return ParseStatus::Success;
}

ParseStatus OR1KAsmParser::parsePCRelTarget(OperandVector &Operands) {
  MCAsmParser &Parser = getParser();
  const MCExpr *Expr = nullptr;
  SMLoc S = getLexer().getLoc();

  if (Parser.parseExpression(Expr))
    return ParseStatus::Failure;

  Operands.push_back(OR1KOperand::createImm(Expr, S, getLexer().getLoc()));
  return ParseStatus::Success;
}

bool OR1KAsmParser::parseOperand(OperandVector &Operands, StringRef Mnemonic) {
  // First, try custom operand parsers generated by TableGen
  ParseStatus Res = MatchOperandParserImpl(Operands, Mnemonic);
  if (Res.isSuccess())
    return false; // successfully parsed
  if (Res.isFailure())
    return true; // hard failure

  // Generic fallbacks
  if (parseRegister(Operands).isSuccess())
    return false;
  if (parseImmediate(Operands).isSuccess())
    return false;

  return Error(getLoc(), "unknown operand");
}

ParseStatus OR1KAsmParser::parseImmediate(OperandVector &Operands) {
  // Accept identifiers, integers, parenthesized, unary +/- etc.
  switch (getLexer().getKind()) {
  default:
    return ParseStatus::NoMatch;
  case AsmToken::LParen:
  case AsmToken::Minus:
  case AsmToken::Plus:
  case AsmToken::Integer:
  case AsmToken::String:
  case AsmToken::Identifier:
    break;
  }

  const MCExpr *Expr = nullptr;
  SMLoc S = getLoc();
  if (getParser().parseExpression(Expr))
    return ParseStatus::Failure;

  SMLoc E = SMLoc::getFromPointer(S.getPointer() - 1);
  Operands.push_back(OR1KOperand::createImm(Expr, S, E));
  return ParseStatus::Success;
}

ParseStatus OR1KAsmParser::parseRegister(OperandVector &Operands) {
  MCRegister Reg;
  SMLoc S, E;
  auto Res = tryParseRegister(Reg, S, E);
  if (!Res.isSuccess())
    return ParseStatus::NoMatch; // allow other operand kinds to be tried
  Operands.push_back(OR1KOperand::createReg(Reg, S, E));
  return ParseStatus::Success;
}

bool OR1KAsmParser::parseInstruction(ParseInstructionInfo &Info, StringRef Name,
                                     SMLoc NameLoc, OperandVector &Operands) {
  // First operand is the mnemonic token
  Operands.push_back(OR1KOperand::createToken(Name, NameLoc));

  // If no operands, allow bare instruction
  if (getLexer().is(AsmToken::EndOfStatement)) {
    getParser().Lex();
    return false;
  }

  // Parse first operand
  if (parseOperand(Operands, Name))
    return true;

  // Remaining operands (comma separated)
  while (parseOptionalToken(AsmToken::Comma))
    if (parseOperand(Operands, Name))
      return true;

  if (getLexer().isNot(AsmToken::EndOfStatement)) {
    SMLoc Loc = getLexer().getLoc();
    getParser().eatToEndOfStatement();
    return Error(Loc, "unexpected token");
  }
  getParser().Lex(); // consume EOL
  return false;
}

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
    SMLoc EL = refineErrorLoc(IDLoc, Operands, ErrorInfo);
    if (ErrorInfo != ~0ULL && ErrorInfo >= Operands.size())
      return Error(EL, "too few operands for instruction");
    return Error(EL, "invalid operand for instruction");
  }
    // Add any custom diagnostics from TableGen with
    // GET_OPERAND_DIAGNOSTIC_TYPES
  }

  report_fatal_error("Unknown match type detected!");
}

ParseStatus OR1KAsmParser::parseDirective(AsmToken DirectiveID) {
  // Handle any target-specific pseudo directives here.
  // Example: .literal, .org, etc. If unrecognized, return NoMatch.
  StringRef IDVal = DirectiveID.getString();
  (void)IDVal;
  return ParseStatus::NoMatch;
}

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOR1KAsmParser() {
  RegisterMCAsmParser<OR1KAsmParser> X(getTheOR1KTarget());
}
