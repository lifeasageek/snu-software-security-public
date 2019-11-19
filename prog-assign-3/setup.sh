#!/bin/bash

set -e

# Download and decompress AFL.
wget http://lcamtuf.coredump.cx/afl/releases/afl-2.33b.tgz
tar -xzvf ./afl-2.33b.tgz

# Download requirements
sudo apt install build-essential libtool automake bison libglib2.0-dev

# Build AFL.
pushd ./afl-2.33b
make -j`nproc`

# Setup AFL's qemu_mode.
pushd qemu_mode
./build_qemu_support.sh
popd

# Replace afl-qemu-trace with a corresponding symbolic link
rm -f afl-qemu-trace
ln -s ./qemu_mode/qemu-2.3.0/`uname -m`-linux-user/qemu-`uname -m` afl-qemu-trace 
