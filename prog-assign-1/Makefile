CLANG_BIN=./llvm/build/bin
INTCHECK_LIB=./build/intcheck/libIntcheckPass.so
INTCHECK_SRC=./pass/intcheck/Intcheck.cpp
RTSRC=./pass/runtime/rtlib.c

all: $(INTCHECK_LIB)

$(INTCHECK_LIB): $(INTCHECK_SRC) $(RTSRC)
	mkdir -p build
	cd build && LLVM_DIR=${CURDIR}/llvm/build cmake ../pass && make

build: $(INTCHECK_LIB)
	${CLANG_BIN}/clang -g \
		-Xclang -load -Xclang $(INTCHECK_LIB) $(RTSRC) \
		./test/malloc-overflow/malloc-overflow.c

run: $(INTCHECK_LIB)
	./a.out 10 10
	./a.out 1000 10000000

clean:
	rm -rf build a.out

.phony:
	clean run
