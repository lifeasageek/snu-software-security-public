#!/bin/bash -e

AFL="afl-2.33b"
CLANG_LLVM_ORIGIN=clang+llvm-3.8.0-x86_64-linux-gnu-ubuntu-14.04
CLANG_LLVM=clang+llvm-3.8.0
CURDIR=$PWD
LINK_PATH=/usr/local/bin
CLANG="clang"
CLANGPP="clang++"
LLVM_CONFIG="llvm-config"

# Download and decompress AFL
if [ ! -e "${AFL}" ]; then
  wget http://lcamtuf.coredump.cx/afl/releases/${AFL}.tgz
  tar -xzvf ./${AFL}.tgz
fi

# Download requirements
sudo apt install build-essential libtool automake bison libglib2.0-dev

# Build AFL
pushd ./afl-2.33b
make -j`nproc`
popd

# Download and decompress LLVM
if [ ! -e ${CLANG_LLVM} ]; then
  wget http://releases.llvm.org/3.8.0/${CLANG_LLVM_ORIGIN}.tar.xz
  tar -xvf ./${CLANG_LLVM_ORIGIN}.tar.xz
  mv ${CLANG_LLVM_ORIGIN} ${CLANG_LLVM}
fi

# Create symbolic links for [clang] & [clang++] & [llvm_config]
sudo rm -rf ${LINK_PATH}/${CLANG}
sudo rm -rf ${LINK_PATH}/${CLANGPP}
sudo rm -rf ${LINK_PATH}/${LLVM_CONFIG}
sudo ln -s ${CURDIR}/${CLANG_LLVM}/bin/${CLANG}       ${LINK_PATH}/${CLANG}
sudo ln -s ${CURDIR}/${CLANG_LLVM}/bin/${CLANGPP}     ${LINK_PATH}/${CLANGPP}
sudo ln -s ${CURDIR}/${CLANG_LLVM}/bin/${LLVM_CONFIG} ${LINK_PATH}/${LLVM_CONFIG}

# Setup AFL's llvm_mode
pushd ./afl-2.33b/llvm_mode
make -j`nproc`
popd

