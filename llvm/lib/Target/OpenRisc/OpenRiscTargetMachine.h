//===-- OpenRiscTargetMachine.h - Define TargetMachine for OpenRisc -*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file declares the OpenRisc specific subclass of TargetMachine.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_LIB_TARGET_OPENRISC_OPENRISCTARGETMACHINE_H
#define LLVM_LIB_TARGET_OPENRISC_OPENRISCTARGETMACHINE_H

#include "OpenRiscSubtarget.h"
#include "llvm/Target/TargetMachine.h"
#include <optional>

namespace llvm {
extern Target TheOpenRiscTarget;

class OpenRiscTargetMachine : public LLVMTargetMachine {
  std::unique_ptr<TargetLoweringObjectFile> TLOF;
public:
  OpenRiscTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      std::optional<Reloc::Model> RM,
                      std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                      bool JIT, bool isLittle);

  OpenRiscTargetMachine(const Target &T, const Triple &TT, StringRef CPU,
                      StringRef FS, const TargetOptions &Options,
                      std::optional<Reloc::Model> RM,
                      std::optional<CodeModel::Model> CM, CodeGenOptLevel OL,
                      bool JIT);

  const OpenRiscSubtarget *getSubtargetImpl(const Function &F) const override;

  TargetPassConfig *createPassConfig(PassManagerBase &PM) override;

  TargetLoweringObjectFile *getObjFileLowering() const override {
    return TLOF.get();
  }

protected:
  mutable StringMap<std::unique_ptr<OpenRiscSubtarget>> SubtargetMap;
};
} // end namespace llvm

#endif // LLVM_LIB_TARGET_OPENRISC_OPENRISCTARGETMACHINE_H
