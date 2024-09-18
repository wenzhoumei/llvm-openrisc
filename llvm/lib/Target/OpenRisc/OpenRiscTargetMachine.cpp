//===- OpenRiscTargetMachine.cpp - Define TargetMachine for OpenRisc ----------===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// Implements the info about OpenRisc target spec.
//
//===----------------------------------------------------------------------===//

#include "OpenRiscTargetMachine.h"
#include "TargetInfo/OpenRiscTargetInfo.h"
#include "llvm/CodeGen/Passes.h"
#include "llvm/CodeGen/TargetLoweringObjectFileImpl.h"
#include "llvm/CodeGen/TargetPassConfig.h"
#include "llvm/MC/TargetRegistry.h"
#include "llvm/Transforms/Scalar.h"
#include <optional>

using namespace llvm;

extern "C" LLVM_EXTERNAL_VISIBILITY void LLVMInitializeOpenRiscTarget() {
  // Register the target.
  RegisterTargetMachine<OpenRiscTargetMachine> A(getTheOpenRiscTarget());
}

static std::string computeDataLayout(const Triple &TT, StringRef CPU,
                                     const TargetOptions &Options,
                                     bool IsLittle) {
  return "E-m:e-p:32:32-i8:8-i16:16-i64:32:64-f32:32-f64:32:64-n32";
}

static Reloc::Model getEffectiveRelocModel(bool JIT,
                                           std::optional<Reloc::Model> RM) {
  if (!RM || JIT)
     return Reloc::Static;
  return *RM;
}

OpenRiscTargetMachine::OpenRiscTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         std::optional<Reloc::Model> RM,
                                         std::optional<CodeModel::Model> CM,
                                         CodeGenOptLevel OL, bool JIT,
                                         bool IsLittle)
    : LLVMTargetMachine(T, computeDataLayout(TT, CPU, Options, IsLittle), TT,
                        CPU, FS, Options, getEffectiveRelocModel(JIT, RM),
                        getEffectiveCodeModel(CM, CodeModel::Small), OL),
      TLOF(std::make_unique<TargetLoweringObjectFileELF>()) {
  initAsmInfo();
}

OpenRiscTargetMachine::OpenRiscTargetMachine(const Target &T, const Triple &TT,
                                         StringRef CPU, StringRef FS,
                                         const TargetOptions &Options,
                                         std::optional<Reloc::Model> RM,
                                         std::optional<CodeModel::Model> CM,
                                         CodeGenOptLevel OL, bool JIT)
    : OpenRiscTargetMachine(T, TT, CPU, FS, Options, RM, CM, OL, JIT, true) {}

const OpenRiscSubtarget *
OpenRiscTargetMachine::getSubtargetImpl(const Function &F) const {
  Attribute CPUAttr = F.getFnAttribute("target-cpu");
  Attribute FSAttr = F.getFnAttribute("target-features");

  auto CPU = CPUAttr.isValid() ? CPUAttr.getValueAsString().str() : TargetCPU;
  auto FS = FSAttr.isValid() ? FSAttr.getValueAsString().str() : TargetFS;

  auto &I = SubtargetMap[CPU + FS];
  if (!I) {
    // This needs to be done before we create a new subtarget since any
    // creation will depend on the TM and the code generation flags on the
    // function that reside in TargetOptions.
    resetTargetOptions(F);
    I = std::make_unique<OpenRiscSubtarget>(TargetTriple, CPU, FS, *this);
  }
  return I.get();
}

namespace {
/// OpenRisc Code Generator Pass Configuration Options.
class OpenRiscPassConfig : public TargetPassConfig {
public:
  OpenRiscPassConfig(OpenRiscTargetMachine &TM, PassManagerBase &PM)
      : TargetPassConfig(TM, PM) {}

  OpenRiscTargetMachine &getOpenRiscTargetMachine() const {
    return getTM<OpenRiscTargetMachine>();
  }

  bool addInstSelector() override;
};
} // end anonymous namespace

bool OpenRiscPassConfig::addInstSelector() {
  addPass(createOpenRiscISelDag(getOpenRiscTargetMachine(), getOptLevel()));
  return false;
}

TargetPassConfig *OpenRiscTargetMachine::createPassConfig(PassManagerBase &PM) {
  return new OpenRiscPassConfig(*this, PM);
}
