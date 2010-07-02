#!/usr/bin/env python
"""
    BinCompare
        Compare two files starting at the first byte.

    ./bincompare.py <file1> <file2> tolerance
    
    tolerance: the number of first "differences" to ignore.
               if 0, dont stop until end of file.

"""
from sys import *

def read_file(file):
    """ 
    Read a binary from filesystem.
    """
    try:
        in_file = open(file, "rb") #binary mode.
    except IOError:
        print "\nYou did something wrong.\nSome kinda file IO error.\n"
        exit(1)
    bytes = []
    bytes = in_file.read() #without a size read() goes to EOF, phat!
    count = len(bytes)
    #print "==>\tRead %d bytes from %s" % (count,file)
    in_file.close()
    return bytes

def compare(file_a, file_b, tolerance=0): 
    """
        file_a and file_b are *not* stream io objects, they
        are character buffers. inefficient but whatevs.
    """
    tolerance = int(tolerance)
    if tolerance == 0: # set tolerance to the size of the largest file.
        if len(file_b) >= len(file_a):
            tolerance = len(file_b)
        else:
            tolerance = len(file_a)        
    diff_switch = 0
    if (len(file_a) != len(file_b)):
        if (len(file_b)) > (len(file_a)):
            len_delta = len(file_b) - len(file_a)
        else:
            len_delta = len(file_a) - len(file_b)
        print "==>\tSizes of files differ by %d bytes." % (len_delta)
        if (len(file_a) < len(file_b)):
            i = len(file_a)
        else:
            i = len(file_b)
    else:
        print "==>\tSizes of files are the same (%d bytes), a good start!" % (len(file_b))
        i = len(file_b)

    for n in range(i):
        if (file_a[n] != file_b[n]):
            print "***** Files differ at byte: 0x%x" % n
            diff_switch = 1
            if (tolerance == 0): 
                break
            tolerance -= 1
        else:
            if ((n == (i-1)) and (diff_switch == 0)):
                print "***** Files contained no differing bytes.\nTHEY LOOK SAME!"
            continue
                 
if __name__ == '__main__':
    if (len(argv) < 3):
        print __doc__
    else:
        if (3 in range(len(argv))):
            compare(read_file(argv[1]), read_file(argv[2]), argv[3])
        else:
            compare(read_file(argv[1]), read_file(argv[2]), 0)
