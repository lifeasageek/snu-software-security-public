# Programming Assignment #1: Simple Integer Overflow Checker using LLVM

- **Due date: 11:59pm, Oct 18, 2019**

- Advanced computer security theories and techniques
- [Dept. of Electrical and Computer Engineering](https://ee.snu.ac.kr/en), [Seoul National University](http://snu.ac.kr/index.html)
- [Instructor: Byoungyoung Lee](https://lifeasageek.github.io/)
- TA: Jinhan Kwak (rhkrwlsgks1@snu.ac.kr)

## NOTES

- To complete this assignment, we strongly encourage you to prepare
your own machine running Ubuntu 16.04 with a root access. Ubuntu 18.04
is not tested, but it should be working without a major issue.

## Introduction

Integer overflows are one of the most well known, a critical type of
vulnerabilities. If overflowed values are used for memory-related
operations, the following program semantics can be broken, which in
turn may result in memory corruptions. For example, if the overflowed
value is used for memory allocation, (i.e., `malloc()`), the memory
will be allocated much smaller than expected.

In fact, this integer overflow itself is not a vulnerability. It is
commonly used for benign cases as well---including hash computations,
image decoding, and many other cases. Thus, one of the key challenge
in detecting harmful integer overflows is in understanding how the
overflown value is impacting following program semantics. Otherwise,
we will suffer from false alarms.

In this programming assignment, we will develop a simple integer
overflow checker. It automatically instruments binary operations in a
target program, which checks whether runtime computation would result
in harmful integer overflows. More specifically, this instrumentation
is carried out only if the resultant value is used by memory
allocation function, `malloc()`.

Before you get started, following articles would be helpful for you to
better understand LLVM.

- [LLVM Architecture](http://www.aosabook.org/en/llvm.html)
- [Writing an LLVM Pass](http://llvm.org/docs/WritingAnLLVMPass.html)

## Base code repository

Clone our git repository,
`https://github.com/lifeasageek/snu-software-security-public`, and all base code is
located in the `prog-assign-1` directory.

- `setup.sh`: Initial environment setup script.
- `Makefile`: makefile for LLVM pass skeleton
- `pass/*`: LLVM pass skeleton files.
- `test/*`: testing files

## How to setup and build

First, install required packages, `clang-3.8` and `cmake` (tested on Ubuntu 16.04).

```
$ sudo apt install clang-3.8 cmake
```

Then run `setup.sh` to download and build `llvm-3.8`. LLVM source code
will be place in `llvm/llvm-3.8.1.src` and it will be built in the
`llvm/build` directory.

```
$ setup.sh
```
 
Next, run `make` command in the `prog-assign-1` directory to build our
integer overflow checker (LLVM pass), `libIntcheckerPass.so`.  If you
see log messages about `libIntcheckPass.so` as below, the build must
be successful.

```bash
$ make
...
[100%] Linking CXX shared module libIntcheckPass.so
...
[100%] Built target IntcheckPass
```

## How to run using our checker

At this point, you should be able to use our checker to build the
program written in `C` or `C++`.  For example, let's take a look at
one of our testing program,
`test/malloc-overflow/malloc-overflow.c`. Notice there is a potential
overflow on `z`, and the value is passed as an allocation size for
`malloc()`.

```c
// test/malloc-overflow/malloc-overflow.c
// All testing annotations (lit and FileCheck) are stripped for simplicity.
// If z is overflowed, we should generate the warning.

#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char** argv) {
    printf("begin main()\n");

    uint x = atoi(argv[1]);
    uint y = atoi(argv[2]);
    uint z = x*y;

    // This malloc() can be vulnerable.
    char *p = malloc(z);

    printf("end main()\n");
    return 0;
}
```

This can be built and run as follows.

```
$ clang-3.8 -g -Xclang -load -Xclang ./build/intcheck/libIntcheckPass.so ./pass/runtime/rtlib.c ./test/malloc-overflow/malloc-overflow.c
Instrument:   %15 = mul i32 %13, %14, !dbg !36

$ ./a.out 10 10
begin main()
[LOG] Computed 64 at line 20
end main()

$ ./a.out 1000 10000000
begin main()
[LOG] Computed 540be400 at line 20
end main()
```

Here, `clang` now uses our function pass `libIntcheckerPass.so` (see
`pass/intcheck/Intcheck.cpp`) to instrument all binary
operations. This intrumentation invokes a runtime function `logop()`
(see `pass/runtime/rtlib.c`).

## TODO: Checking integer overflow conditions

If you look at two source code files, `Intcheck.cpp` and `rtlib.c`, we
left `TODO` comments that you are expected to fill up for this
assignment. In short, you need to implement a simple intra-procedural
analysis on binary operations to see if a resultant value has
dependency with an allocation function, `malloc()` (see
`shouldCheckOverflow()` in `pass/intcheck/Intcheck.cpp`). You will
also need to pass operand values to `logop()`, and further check
integer overflow conditions.

If your implementation is correct, running a sample program
(`malloc-overflow.c`) should output as follows.

```
$ ./a.out 10 10
begin main()
end main()

$ ./a.out 1000 10000000
begin main()
[WARNING] Integer overflow detected at line 20
end main()
```

## Testing

Testing can be done through the `test/run-test.sh` file. This testing
is based on `lit` and checks outputs with
[FileCheck](http://llvm.org/docs/CommandGuide/FileCheck.html).

Below shows if your implementation is incomplete (failing both tests).

```
$ ./test/run-test.sh 
-- Testing: 2 tests, 2 threads --
FAIL: INTCHECK :: benign-overflow/benign-overflow.c (1 of 2)
FAIL: INTCHECK :: malloc-overflow/malloc-overflow.c (2 of 2)
Testing Time: 0.16s
********************
Failing Tests (2):
    INTCHECK :: benign-overflow/benign-overflow.c
    INTCHECK :: malloc-overflow/malloc-overflow.c
        
  Unexpected Failures: 2
```

If all your TODO is correctly done, it should pass both testing.

```
$ ./test/run-test.sh 
-- Testing: 2 tests, 2 threads --
PASS: INTCHECK :: malloc-overflow/malloc-overflow.c (1 of 2)
PASS: INTCHECK :: benign-overflow/benign-overflow.c (2 of 2)
Testing Time: 0.20s
  Expected Passes    : 2
```

## Submission

- edit the files in the "pass" folder
- run the script "prepare-submit.sh"
- rename the file "assign1.tar.gz" to "[Student-number]_assign1.tar.gz" (e.g., 2019-12345_assign1.tar.gz)
- upload to eTL
