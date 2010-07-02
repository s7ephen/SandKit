NTVDM or "How to cheat at TinyPE Challenge ;-)"
===============================================

Ok (for the record) I dont really consider this
a serious entry for the TinyPE competition, its just a joke ;-)

During my "Escaping The Sandbox" talk while talking
about the different subsystems in the NT Kernel I 
tended to go off on a tangent and use NTVDM an WoW16 as examples.
One example that I mention is my ebfe.exe concept which is to either
compile ebfe.asm from source or simply to write the bytes directly to
disk withi something like:
    >>> f_h = open("/tmp/ebfe.exe",'wb')
    >>> f_h.write("\xeb\xfe")
    >>> f_h.close()
Anyway, what is interesting is that when you "double click" this exe
from windows Explorer, it executes! (You can verify by watching the 
CPU percentage peg to 100% in taskmgr.exe)

Anyway the thing that makes this possible is that the loader is 
detecting that the instruction width of the file is only 16-bits and
that it doesn't have a valid PE header. Thus it runs it using a "userspace"
loader.


