#!/bin/bash -e

# Download llvm
mkdir llvm
pushd llvm
wget http://llvm.org/releases/3.8.1/llvm-3.8.1.src.tar.xz
tar xf llvm-3.8.1.src.tar.xz

# Build llvm
mkdir build
pushd build
cmake ../llvm-3.8.1.src
make -j`nproc`
popd
