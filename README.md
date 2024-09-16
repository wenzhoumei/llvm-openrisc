# LLVM OpenRISC Backend
This project provides a backend for the OpenRISC architecture.

## Features
- ORBIS32 class I instruction set support

## TODO
- Integrated assembler
- Floating-point support
- GCC interoperability

## Setup and Compilation
### Steps to Compile
1. Clone the LLVM project and this backend:
   ```
   git clone https://github.com/llvm/llvm-project.git
   cd llvm-project
   ```

2. Create a build directory:
   ```
   mkdir build
   cd build
   ```

3. Configure the build:
   ```
   cmake -G Ninja -DLLVM_ENABLE_PROJECTS="clang;llvm" -DLLVM_EXPERIMENTAL_TARGETS_TO_BUILD="OpenRisc" -DCMAKE_BUILD_TYPE=Debug -DLLVM_USE_LINKER=<linker> ../llvm
   ```

4. Build the project:
   ```
   ninja -v -j<num-jobs>
   ```

### Usage
To compile code using llc for the OpenRISC backend, use the following command:
   ```
   llc -march=openrisc -S -O2 <source-file.ll>
   ```

### Tests
My tests for this backend are currently located under the following directory:
   ```
   llvm-openrisc/llvm/test/CodeGen/OpenRisc/Personal
   ```

# The LLVM Compiler Infrastructure

[![OpenSSF Scorecard](https://api.securityscorecards.dev/projects/github.com/llvm/llvm-project/badge)](https://securityscorecards.dev/viewer/?uri=github.com/llvm/llvm-project)
[![OpenSSF Best Practices](https://www.bestpractices.dev/projects/8273/badge)](https://www.bestpractices.dev/projects/8273)
[![libc++](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml/badge.svg?branch=main&event=schedule)](https://github.com/llvm/llvm-project/actions/workflows/libcxx-build-and-test.yaml?query=event%3Aschedule)

Welcome to the LLVM project!

This repository contains the source code for LLVM, a toolkit for the
construction of highly optimized compilers, optimizers, and run-time
environments.

The LLVM project has multiple components. The core of the project is
itself called "LLVM". This contains all of the tools, libraries, and header
files needed to process intermediate representations and convert them into
object files. Tools include an assembler, disassembler, bitcode analyzer, and
bitcode optimizer.

C-like languages use the [Clang](https://clang.llvm.org/) frontend. This
component compiles C, C++, Objective-C, and Objective-C++ code into LLVM bitcode
-- and from there into object files, using LLVM.

Other components include:
the [libc++ C++ standard library](https://libcxx.llvm.org),
the [LLD linker](https://lld.llvm.org), and more.

## Getting the Source Code and Building LLVM

Consult the
[Getting Started with LLVM](https://llvm.org/docs/GettingStarted.html#getting-the-source-code-and-building-llvm)
page for information on building and running LLVM.

For information on how to contribute to the LLVM project, please take a look at
the [Contributing to LLVM](https://llvm.org/docs/Contributing.html) guide.

## Getting in touch

Join the [LLVM Discourse forums](https://discourse.llvm.org/), [Discord
chat](https://discord.gg/xS7Z362),
[LLVM Office Hours](https://llvm.org/docs/GettingInvolved.html#office-hours) or
[Regular sync-ups](https://llvm.org/docs/GettingInvolved.html#online-sync-ups).

The LLVM project has adopted a [code of conduct](https://llvm.org/docs/CodeOfConduct.html) for
participants to all modes of communication within the project.
