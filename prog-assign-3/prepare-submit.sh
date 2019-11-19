#!/bin/bash

set -e

ASSIGN=assign3.tar.gz
DIFF=assign3.diff

BASE=`mktemp -d`
echo "[*] Using temporal directory base [$BASE]"

rm -f $ASSIGN

# Download and decompress QEMU in $BASE
pushd $BASE
wget http://wiki.qemu-project.org/download/qemu-2.3.0.tar.bz2
echo "[*] Downloaded QEMU"
tar -xjf ./qemu-2.3.0.tar.bz2
echo "[*] Decompressed QEMU"
popd

# Make sure the directory location.
pushd ./afl-2.33b
pushd ./qemu_mode
popd
popd

echo "[*] Please wait. In preparation ..."

diff -N -u -x "*.[o|d]" -x "*timestamp" -x "*.lo" -r $BASE afl-2.33b/qemu_mode > $DIFF || echo ""
diff -u -x "*.[o|d]" -x "*timestamp" -x "*.lo" -r $BASE afl-2.33b/qemu_mode > $DIFF.small || echo ""

echo "[*] Diff result is saved to [$DIFF] and [$DIFF.small]"

tar -czf $ASSIGN $DIFF $DIFF.small

echo "[*] GOOD! Your package [$ASSIGN] is ready."
echo "[*] Please submit [$ASSIGN] through the submission site."

rm -rf $BASE
