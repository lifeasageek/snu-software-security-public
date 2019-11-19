AFL_DIR=${CURDIR}/../afl-2.33b
AFL_FUZZ=${CURDIR}/../afl-2.33b/afl-fuzz
AFL_QEMU=${CURDIR}/../afl-2.33b/afl-qemu-trace

all: bin-weak bin-strong

qemu:
	(cd ${CURDIR}/../afl-2.33b/qemu_mode/qemu-2.3.0 && make)

bin-weak: test.c
	gcc -std=c99 -O0 $< -D COND_WEAK -o $@

bin-strong: test.c
	gcc -std=c99 -O0 $< -o $@

runafl-weak: bin-weak
	${AFL_FUZZ} -i ${CURDIR}/input -o ${CURDIR}/output-weak -Q -- ${CURDIR}/$< @@

checkafl-weak: bin-weak
	${AFL_QEMU} -d op $< ./input/input.txt 2>$<.tc

runafl-strong: bin-strong
	${AFL_FUZZ} -i ${CURDIR}/input -o ${CURDIR}/output-strong -Q -- ${CURDIR}/$< @@

checkafl-strong: bin-strong
	${AFL_QEMU} -d op $< ./input/input.txt 2>$<.tc

clean:
	rm -rf bin-weak bin-strong
	rm -rf bin-weak.tc bin-strong.tc
