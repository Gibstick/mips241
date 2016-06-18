#!/usr/bin/env python3

import fileinput
import struct
import sys
import signal

signal.signal(signal.SIGINT, lambda x, y: sys.exit(0))

def get_skel(s):
    """Get a tuple representing an instrution skeleton.
    
    Args:
        s: String of chars in {0, 1, x} of the skeleton
    Returns:
        Tuple (before, length, after), where 
        - before is the number before the mask
        - length is the length of x's in the mask
        - after is the number after the mask
    """
    i = 0
    # get number before x's
    before = 0
    while i < len(s):
        if s[i] != 'x':
            assert s[i] == '0' or s[i] == '1'
            before += before
            before += int(s[i])
        else:
            break
        i += 1
    # get number of x's
    xlen = 0
    while i < len(s):
        if s[i] == 'x':
            xlen += 1
        else:
            break
        i += 1
    # get number of 0s after x's
    zerolen = 0
    while i < len(s):
        if s[i] == '0':
            zerolen += 1
        else: 
            break
        i += 1
    # get number afer x's
    after = 0
    while i < len(s):
        assert s[i] == '0' or s[i] == '1'
        after += after
        after += int(s[i])
        i += 1

    return (before, xlen, zerolen, after)

def gen_from_skel(skel):
    """
    Iterator for all possible instructions, given a skeleton.
    See get_skel(s).

    Args:
        skel: skel, from get_skel
    Yields:
        A next possible instruction in machine code, as a word. 
    """
    (before, xlen, zerolen, after) = skel
    increment = 1 << (after.bit_length() + zerolen)
    result = (before << (xlen + zerolen + after.bit_length())) + after

    for i in range(1 << xlen):
        yield result
        result += increment

for line in fileinput.input():
    line = line.strip()
    for instruction in gen_from_skel(get_skel(line)):
        sys.stdout.buffer.write(struct.pack('>I', instruction))
