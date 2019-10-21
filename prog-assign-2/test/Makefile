AFL_LLVM=${CURDIR}/../afl-2.33b/afl-clang-fast
AFL_FUZZ=${CURDIR}/../afl-2.33b/afl-fuzz

all: bin-weak bin-strong

llvm:
	(cd ${CURDIR}/../afl-2.33b/llvm_mode && make)

bin-weak: test.c 
	${AFL_LLVM} -std=c99 -D COND_WEAK $< -o $@

runafl-weak: bin-weak
	${AFL_FUZZ} -i ${CURDIR}/input -o ${CURDIR}/output-weak ${CURDIR}/$< @@

bin-strong: test.c
	${AFL_LLVM} -std=c99 -O0 $< -o $@

runafl-strong: bin-strong
	${AFL_FUZZ} -i ${CURDIR}/input -o ${CURDIR}/output-strong ${CURDIR}/$< @@

.phony: bin-weak bin-strong runafl-weak runafl-strong clean

clean:
	rm -rf bin-weak bin-strong 
	rm -rf output-weak output-strong
