#!/usr/bin/env python2

import os
import subprocess
import socket
import string
import platform

URL = "https://raw.githubusercontent.com/lifeasageek/snu-software-security-public/master/lab-assign-setup"

# debugging
if "URL" in os.environ:
    URL = os.environ["URL"]

def curl(url, *args):
    try:
        html = subprocess.check_output(["curl"] + list(args) + [url],
                                       universal_newlines=True)
    except subprocess.CalledProcessError as e:
        html = e.output
    return html.strip()

def get_env():
    return "%s-%s" % (platform.architecture()[0][0:2], platform.release())


def shrun(cmds):
    try:
        output = subprocess.check_output(cmds,
                                         universal_newlines=True)
    except subprocess.CalledProcessError as e:
        output = e.output
    return output

#
# flag related
#
def normalize_flag(flag):
    flag = "".join(flag.split())
    if not all (c in string.hexdigits for c in flag):
        print "[!] warn: flag includes a non-hexdigit char\n%s" % flag

    return flag

def read_flag():
    flag = ""
    while True:
        bits = input()
        flag += bits.strip().replace("\n", "")

        if len(flag) >= 512*2:
            break

    if len(flag) != 512*2:
        print "[!] warning: len(flag) = %d, should be 1024" % len(flag)
        flag = flag[:512*2]
    return flag
