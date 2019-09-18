# Lab Assignment #1: Exploiting Simple Stack overflow

- **Due date: 11:59pm, Sep 28, 2019**
- TA: Jinhan Kwak (rhkrwlsgks1@snu.ac.kr)

In this assignment you will learn how to exploit a simple
stack overflow in the `target` binary (i.e., `lab-assign-1/target` in our git
repository `https://github.com/lifeasageek/snu-software-security-public`).
Specifically, you will need to reverse-engineer `target` in order to
(1) understand how the stack overflow happens, (2) manipulate a return
address value, and (3) achieve the final exploitation goal,
reading the `flag`.

## Instruction

Before getting started, follow the
[setup instruction](https://github.com/lifeasageek/snu-software-security-public/blob/fall-19/lab-assign-setup/README.md)
to setup the working environment. After setting up the environment,
you can run the following commands to run the `target` binary.

```sh
# ssh to VM
$ vagrant ssh

[vm]:~$ cd /vagrant/snu-software-security-public/lab-assign-1

# Run the target binary will ask you to enter stdin inputs.
[vm]:/vagrant/snu-software-security-public/lab-assign-1$ ./target
Please input something...
```

## Exploitation Goal: Reading a Flag 

In this lab assignment (as well as all the following lab assignments),
your exploitation goal is to read a flag. This `flag` can be retrieved
by reading the file, `/proc/flag`. If you simply tried to read this
`/proc/flag` without going through a proper exploitation process, it
will return a meaningless flag.

```sh
# This simple flag reading attempt will return a meaningless flag 
# (NOT A VALID FLAG)
[vm]:~$ cat /proc/flag 
F38FE749E36CD0437AD0062D6836C393618FCC4A412FDE666C89EB41AC0814D5
                  >>> NOT A VALID FLAG  <<<
l2AF0AEC61229004D9E8CF6F4E3622419A37F6D410526951D6EB2F639DBEA49AD
```

However, if you truly exploited (i.e., reading `/proc/flag` by
hijacking the control flows of `target`), it will generate valid, long
flag strings (at least 5 lines), which encrypts various information on
how you exploited. Using this information, we can later verify whether
you truly exploited or cheated the exploitation process:(

For example, once we run the reference exploit, `exploit.py` (of
course this is NOT include in the repository), following outputs are
shown. Since this flag sting can be randomly changed, you may assume
that you got a valid flag as long as your flag strings do not include
the string
`"NOT A VALID FLAG"`.

```sh
# Running the target binary with a reference exploit (i.e.,
# exploit.py), a flag is dumped.
[vm]:/vagrant/snu-software-security-public/lab-assign-1$ ./exploit.py|./target
Please input something...
This is your flag:

5CA37AA3E5B205B01DAF416952A8291B4EDA28CB96D0FC0B7056B0199F5160D3
# More flags strings will be shown here.
```

## Submission

Make your own tarball, named `lab1.tar.gz`, and submit it through
eTL. Your tarball must include following files.

- `flag`: a flag that you obtained from exploitation
- `README`: shortly describes how to run the exploit (less than 5 sentences!)
- `exploit.*`: your own exploit source code


