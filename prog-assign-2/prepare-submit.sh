#!/bin/bash -e

ASSIGN=assign2.tar.gz
PASS_DIR=./afl-2.33b/llvm_mode

tar -czf ${ASSIGN} ${PASS_DIR}
