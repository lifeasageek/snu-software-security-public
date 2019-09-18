# Lab Assignment #2: Exploiting Heap Overflow

- **Due date: 11:59pm, Oct 2, 2019**
- TA: Jinhan Kwak (rhkrwlsgks1@snu.ac.kr)

In this lab assignment, you will exploit a heap overflow in the
`target` binary (i.e., `lab-assign-2/target` in our in our git
repository `https://github.com/lifeasageek/snu-software-security-public`).
Note, the memory allocate used in the binary is dlmalloc
([the source code](http://gee.cs.oswego.edu/pub/misc/malloc-2.6.1.c)).
Before you get started, you may want to check following articles
first.

- [A Memory Allocator (dlmalloc)](http://gee.cs.oswego.edu/dl/html/malloc.html)
- [Understanding the Heap & Exploiting Heap Overflows](http://www.mathyvanhoef.com/2013/02/understanding-heap-exploiting-heap.html)

## Instruction & Exploitation Goal

The same as the [first lab assignment](https://github.com/lifeasageek/snu-software-security-public/tree/fall-19/lab-assign-1).

## Submission

Make your own tarball, named `lab2.tar.gz`, and submit it through
eTL. Your tarball must include following files.

- `flag`: a flag that you obtained from exploitation
- `README`: shortly describes how to run the exploit (less than 5 sentences!)
- `exploit.*`: your own exploit source code
