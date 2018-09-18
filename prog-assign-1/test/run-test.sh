#!/bin/bash -e

CURDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
LLVM_BIN_DIR=${CURDIR}/../llvm/build/bin
LIT=${CURDIR}/../llvm/llvm-3.8.1.src/utils/lit/lit.py

PATH=${LLVM_BIN_DIR}:${PATH} ${LIT} ${CURDIR}

