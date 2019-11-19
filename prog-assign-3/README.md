# Programming Assignment #3: Weakening Strong Constraints for AFL using QEMU

- **Due Date: 11:59pm, Dec 13, 2019**

- Software Security
- [Dept. of Electrical and Computer Engineering](http://ece.snu.ac.kr/en), [Seoul National University](http://snu.ac.kr/index.html)
- Instructor: [Byoungyoung Lee](https://lifeasageek.github.io/) (byoungyoung@snu.ac.kr)
- TA: Jinhan Kwak (rhkrwlsgks1@snu.ac.kr)

## Introduction

Fuzz testing or fuzzing is a software testing technique popularly used
to discover software vulnerabilities. It generates a massive number of
random inputs to run a target program in hopes that the program
crashes for a certain input.  This crash event may have a strong
potential to be a vulnerability, as the crash itself manifests some
unexpected behaviors of the target program.  In practice, most fuzzers
are targeted to discover memory corruption vulnerabilities.

In this programming assignment, we will try to improve one of the most
well known fuzzer, [AFL](http://lcamtuf.coredump.cx/afl/). While AFL
provides many interesting features, one of the most outstanding
feature is its design in using a genetic algorithm to efficiently
explore new program states in a target binary --- maintaining an
internal state matrix and updating the matrix according to something
similar to [n-grams](https://en.wikipedia.org/wiki/N-gram) of basic
blocks (so you can say AFL leveragesa a simple yet effective machine
learning technique). Before you start this assignment, we strongly
recommend you read through the README documentation of AFL and run it
for some programs
[tutorial](https://fuzzing-project.org/tutorial3.html).

## Motivation - Strong constraints are not fuzzer friendly

Consider the following code snippet.

```c
if (x == 0x12345678) {
  // then-block
  // Assume there is some vulnerable code here.
}
```

Assuming x is the input variable of a target program, it is highly
unlikley that randomly mutating x would hit the then-block condition,
as the chance of satisfying the given predicate is about 1/(2^32). As
such, fuzzers including AFL, which generally takes a random input
mutation strategy, cannot trigger the above condition, and thus fail
to discover a vulnerability in then-block. Note, symbolic execution
techniques, including
[KLEE](http://llvm.org/pubs/2008-12-OSDI-KLEE.pdf) or
[angr](http://angr.io/), do not suffer from this issue in general, and
we will cover these techniques in the class later.

Now let's consider semantically the same, but a bit tweaked code.

```c
if (((x >> 24) & 0xff) == 0x12) {
  // match1-block
  if (((x >> 16) & 0xff) == 0x34) {
    // match2-block
    if (((x >> 8) & 0xff) == 0x56) {
      // match3-block
      if (((x >> 0) & 0xff) == 0x78) {
        // then-block
      }
    }
  }
}
```

As you can see, while it checks the condition for four times, the code 
basically performs the same logic --- execute then-block only if the variable 
x is 0x12345678. While this simple transformation may seem pointless, it may 
dramatically improve AFL in exploring the then-block. The secret lies in the 
fact how AFL mutates the input. Once AFL hits match1-block, it will keep the 
corresponding input and mutate it again until it hits match2-block. This process 
will be repeated until it hits then-block. Therefore, a single event drawn 
from 2^32 space is transformed into four dependent events drawn from 2^8 space 
following the additional rule in probabilistic, resulting in a significant 
search space reduction.

## Assignment - weakening strong constraints in QEMU

Now we begin to describe how we automatically transform the target program 
to benefit AFL from this idea, which is the goal of this programming 
assignment. Specifically, we will take binary-based transformation, which 
doesn't require us to access the source code of a target program.
In fact, AFL already implements this binary-based transformation using
[QEMU](https://wiki.qemu.org/Main_Page).
Instead of natively run the target program, AFL's QEMU-mode emulates 
the program execution with the help of QEMU and instruments all necessary 
code on-the-fly through QEMU. In order to understand how this works, refer 
following links:

- [QEMU Internals](https://qemu.weilnetz.de/doc/2.7/qemu-tech-20160903.html)
- [The list of changes](https://github.com/mirrorer/afl/tree/master/qemu_mode/patches) 
that AFL made to enable this instrumentation.

We will make similar changes as AFL's QEMU-mode did, but in a slightly 
different way --- instead of instrumenting at the basic block level, 
we will do the instrumentation at the instruction-by-instruction level. 
This is largely because our weakening idea needs to instrument and convert 
a branch instruction, which is difficult to be done at the basic block level. 
This instruction-by-instruction level instrumentation can be done with 
[Tiny Code Generator (TCG)](https://wiki.qemu.org/Documentation/TCG).

Particularly for this project, you may want to focus on the function, 
`tcg_gen_brcond_i64()`. This function is invoked when QEMU translates a 
conditional branch instruction, and you can run your additional logic by 
invoking a runtime helper function.
For example, the reference implementation does the following:

- Created a new runtime helper function in `qemu_mode/qemu-2.3.0/tcg-runtime.c`.
According to the branch condition, this function updates the status of AFL 
by writing to the `afl_area_ptr` matrix.

- Modified `tcg_gen_brcond_i64()` in `qemu_mode/qemu-2.3.0/tcg/tcg-op.c` 
to invoke a new runtime helper function.

For your reference, the following shows a list of changed files (diff stats) 
to implement this assignment.

```
afl-1.86b/qemu_mode/patches/afl-qemu-cpu-inl.h    |  2 +-
afl-1.86b/qemu_mode/qemu-2.3.0/tcg-runtime.c      | 35 +++++++++++++++++++++++++++++++++++
afl-1.86b/qemu_mode/qemu-2.3.0/tcg/tcg-op.c       | 24 ++++++++++++++++++++++++
afl-1.86b/qemu_mode/qemu-2.3.0/tcg.tcg-runtime.h  |  2 ++

4 files changed, 62 insertions(+), 1 deletion(-)
```

Note, it is fine if your transformation only supports the specific 
architecture (i.e., x86 or x86-64), or takes place in the front-end 
(rather than in the backend). Moreover, the reference implementation is just 
one possible way to implement this functionality, and feel free to take 
some other approaches if you think it is more general and efficient.

## How to Setup, Build and Test

Descriptions below assume that you have an access to the Linux-based
machine with root privilege. If you do not have one, please read
[this](https://github.com/lifeasageek/snu-software-security-public/tree/fall-19/lab-assign-setup).

We provide a basic setup script for this assignment. You can follow the steps
described below to setup the `AFL` with `qemu_mode`.

```
$ git clone -b fall-19 https://github.com/lifeasageek/snu-software-security-public
$ cd snu-software-security-public/prog-assign-3
$ ./setup.sh
```

Once the build is successful, now your system must be ready to run AFL with
QEMU. Our testing code (i.e., a sample vulnerable program to be fuzzed) is
located in `snu-software-security-public/prog-assign-3/test/test.c`.

```c
// test.c
#include <stdio.h>
#include <string.h>

#define MSG_HELO "> Started.\n"
#define MSG_VULN "> Vulnerability triggered.\n"
#define MSG_EXIT "> Exiting.\n"

#define BUFLEN 4

void crash() {
  // Intentionally trigger the crash to simulate a vulnerability.
  printf(MSG_VULN);

  int *y = NULL;
  *y = 0x1234;
}

int main(int argc, char *argv[]) {
  char c;
  FILE *file;
  char buf[BUFLEN];

  if (argc <= 1) {
    fprintf(stderr, "[-] ERROR: Not valid arguments\n");
    return -1;
  }

  printf(MSG_HELO);

  file = fopen(argv[1], "r");
  if (!file) {
    fprintf(stderr, "[-] ERROR: Failed to open a file [%s]\n", argv[1]);
    return -1;
  }

  for (int i=0; i < BUFLEN; i++) {
    c = getc(file);
    if (c == EOF)
      break;
    buf[i] = c;
  }

  fclose(file);

#ifdef COND_WEAK
  if (buf[0] == 0x54) {
    // Weak condition. Original AFL can easily solve.
    crash();
  }
#else
  unsigned int *x = (unsigned int *)buf;
  if (*x == 0x54534554) { // x == "TEST"
    // Strong condition. Original AFL cannot easily solve.
    crash();
  }
#endif

  printf(MSG_EXIT);
  return 0;
}
```

You can build this testing program with `make` command, which generates
two executables, **bin-weak** and **bin-strong**, depending on whether
the definition (`COND_WEAK`) is given or not, respectively.

As the branch condition is already weak enough (as it is only matching a
one-byte), the original AFL can easily solve **bin-weak** (i.e., AFL can 
find a crashing input) in a very short time. For example, if you run AFL
with the following command, you will see AFL immediately discovers crashing
inputs.

```
$ cd test
# Compile the test code
$ make
# Run AFL on the binary, bin-weak
$ make runafl-weak
```

The image below shows the running result of AFL using the command 
`make runafl-weak`. As you can see, AFL found total 140 crashes
(with 2 unique crashes --- refer AFL's README for more details on unique crashes)
in 20 seconds.

![afl-weak](./afl-weak.png)

However, in the case of `bin-strong`, as the branch condition is strictly
restricted, the original AFL cannot solve it in a reasonable time.
From our testing, no crashes were observed in an hour.

```
# Run AFL on the binary, bin-strong.
$ make runafl-strong
```

Now, your job is to modify the AFL's qemu-mode so that the AFL can solve 
`bin-strong` more efficiently. Note, the performance of fuzzer is highly
depending on the initial inputs (so called seeding inputs), so you
should **NOT** change the given initial input (test/input/input.txt) for
fair testing.

## Submission

- Run `prepare-submit.sh` script
- Upload to eTL
