#!/bin/bash -e

# Download llvm
mkdir llvm
pushd llvm
git clone -b llvmorg-9.0.0 https://github.com/llvm/llvm-project.git

# Build llvm
mkdir build
pushd build
cmake -DLLVM_TARGETS_TO_BUILD=X86 \
      -DLLVM_ENABLE_PROJECTS="clang;compiler-rt" \
      -DCMAKE_BUILD_TYPE=Release \
      -G "Unix Makefiles" \
      ../llvm-project/llvm
make -j`nproc`

popd
popd
