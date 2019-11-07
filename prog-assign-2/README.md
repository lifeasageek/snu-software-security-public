# Programming Assignment #2: Weakening Strong Constraints using LLVM

- **Due Date: 11:59pm, Nov 15, 2019**

- Software Security
- [Dept. of Electrical and Computer Engineering](https://ee.snu.ac.kr/en), [Seoul National University](http://snu.ac.kr/index.html)
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

## Motivation

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

## Assignment

In this programming assignment, we will transform the target program
to benefit AFL from the idea above. To automatically achieve this, we
will take the llvm-based transformation. Fortunately, AFL already
implemented llvm-based code instrumentation called AFL's
llvm-mode. When AFL compiles the target source code, AFL's llvm-mode
can instrument the target program code with the llvm-IR level
instrumentation. So, we can insert the additional code transformation
logic using AFL's llvm-mode.

Before you get started, following articles would be helpful for you to better 
understand LLVM.

- [LLVM Architecture](http://www.aosabook.org/en/llvm.html)
- [Writing an LLVM Pass](http://llvm.org/docs/WritingAnLLVMPass.html)

## How to setup, build and test

Descriptions below assume that you have an access to the Linux-based
machine with root privilege. For this assignment, you must use
[vagrant](https://github.com/lifeasageek/snu-software-security-public/tree/fall-19/lab-assign-setup).

We provide a setup script for this assignment. Clone the github repository
and then execute `setup.sh`.

```
$ git clone -b fall-19 https://github.com/lifeasageek/snu-software-security-public
$ cd snu-software-security-public/prog-assign-2
$ ./setup.sh
```

A `setup.sh` script installs the default requirements, downloading and
decompressing AFL and LLVM. If `setup.sh` is done successfully, you
will be able to see the following messages.

```
echo 0 | ../afl-showmap -m none -q -o .test-instr0 ./test-instr
echo 1 | ../afl-showmap -m none -q -o .test-instr1 ./test-instr
[+] All right, the instrumentation seems to be working!
[+] All done! You can now use '../afl-clang-fast' to compile programs.
```

We provide a test code that allows you to check your instrumentation works.

```c
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

  for (int i = 0; i < BUFLEN; i++) {
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

You can generate two kinds of binaries from this test code,
**bin-weak** and **bin-strong**. A bin-weak contains weak branch
conditions (one-byte comparison), so the original AFL can easily solve
the bin-weak (i.e., AFL finds a crashing input) in a fairly short
time. For example, if your run AFL with the following command, you can
see that AFL finds unique crashes very quickly.

```
$ cd test
# Compile the test code
$ make
# Run AFL on the binary, bin-weak
$ make runafl-weak
```

However, the original AFL cannot find the crashing input for bin-strong in 
a reasonable time. You can check this with the following command.

```
# Run AFL on the binary, bin-strong
$ make runafl-strong
```

Your job is to modify the AFL's llvm-mode so that the AFL can find the
crashing inputs in a reasonable time (**less than 1 minute**).

## Submission

- Run the `prepare-submit.sh` script
- Rename the file `assign2.tar.gz` to `[your student number]_assign2.tar.gz`
(e.g., `2019-12345_assign2.tar.gz`)
- Upload to eTL

## Notice

- The performance of fuzzers including AFL are highly depending on the initial
input (seed input). So, you should **NOT** change the given seed input
(`test/input/input.txt`).

- Please note that the above test code is just for checking. We may
evaluate whether your instrumentation works for other test cases.

- We assume that your implementation only modifies the AFL's
llvm-mode. If not, please upload all files that you modified as well
as your own README (which describes how to run your code, etc.) to
eTL.
