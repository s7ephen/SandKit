#--------------------------------
# LiteDbg
#   This is a "lite" library that contains a class to help with
#   "debuggerish" tasks.
#       stephen@sa7ori.org 2008/2009
#--------------------------------
import sys
from ctypes import *
from ctypes.wintypes import DWORD, HANDLE
from binascii import hexlify
from binascii import unhexlify

def ps(process_name=None):
    """
    Find a process by name, or if "process_name" is None just list them.
    """
    PROCESS_QUERY_INFORMATION = 0x0400
    PROCESS_VM_READ = 0x0010
    BIG_ARRAY = DWORD * 4096
    processes = BIG_ARRAY()
    needed = DWORD()
    pids = []
    result = windll.psapi.EnumProcesses(processes, sizeof(processes), addressof(needed))
    if not result:
        return pids
    num_results = needed.value / sizeof(DWORD)
    for i in range(num_results):
        pid = processes[i]
        process = windll.kernel32.OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, pid)
        if process:
            module = HANDLE()
            result = windll.psapi.EnumProcessModules(process, addressof(module), sizeof(module), addressof(needed))
            if result:
                name = create_unicode_buffer(1024)
                result = windll.psapi.GetModuleBaseNameW(process, module, name, len(name))
                if process_name == None:
                    print ("%d : %s") % (pid,name.value)
                    pids.append(pid)
                elif name.value.startswith(process_name):
                    print ("%d : %s") % (pid,name.value)
                    pids.append(pid)
                windll.kernel32.CloseHandle(module)
            windll.kernel32.CloseHandle(process)
    return pids

def hexdump(src, length=16):
    """
        A function to emulate "hexdump -C" output.
    """
    N=0; result=''
    FILTER=''.join([(len(repr(chr(x)))==3) and chr(x) or '.' for x in range(256)])
    while src:
        s,src = src[:length],src[length:]
        hexa = ' '.join(["%02X"%ord(x) for x in s])
        s = s.translate(FILTER)
        result += "%08X:  %-*s  |%s|\n" % (N, length*3, hexa, s)
        N+=length
    print result

class LiteDbg:
    """
        A class to house some (but not all) of the debugger-ish
        functions that we need. 
    """

    def __init__(self, pid):
        # --------- 
        # CONSTANTS:
        # --------- 
        # Some Constants for use by member functions.
        # "self." is soooo ugly for this. I hate to admit it, but Ruby does this better.
        # All Memory Protection Constants come from:
        # http://msdn.microsoft.com/en-us/library/aa366786(VS.85).aspx
        self.PAGE_EXECUTE_READWRITE = 0x40
        self.PAGE_READWRITE = 0x04
        self.PROCESS_ALL_ACCESS = (0x000F0000 | 0x00100000 | 0xFFF)
        self.VIRTUAL_MEM = (0x1000 | 0x2000)
        self.PROCESS_TERMINATE = 0x0001
        self.PROCESS_QUERY_INFORMATION = 0x0400
        self.PROCESS_VM_READ = 0x0010
        self.pid = pid
        self.h_process = None #a placeholder for the process handle
        self.attach()

    def __del__(self):
        self.detach()
        pass
 
    def attach(self):
        h_process = windll.kernel32.OpenProcess(self.PROCESS_ALL_ACCESS, False, int(self.pid))
        if not h_process:
            print "Could not attach to PID: %d" % self.pid
            sys.exit(0)
        else:
            print "Attached to PID: %d" % self.pid
            self.h_process = h_process

    def detach(self):
        windll.kernel32.CloseHandle(self.h_process)
        print "Detached from PID: %d" % self.pid

    def virtual_protect(self, base_addr, size, protection):
        saved_page_perms = c_ulong(0)
        windll.kernel32.VirtualProtectEx(self.h_process, base_addr, size, protection, byref(saved_page_perms))
        #print "VirtualProtectEx(): 0x%x (old perms) 0x%x (new perms)" % (saved_page_perms.value, protection)
        return saved_page_perms.value

    def read_memory(self, read_addr, read_len):
        read_buf = create_string_buffer(read_len) 
        read_count = c_int(0)
        old_page_perms = self.virtual_protect(read_addr, read_len, self.PAGE_EXECUTE_READWRITE)
        if not windll.kernel32.ReadProcessMemory(self.h_process, read_addr, read_buf, read_len, byref(read_count)):
            if read_count.value <= 0:    
                #print GetLastError()
                print FormatError()
                raise "Memory Read Error"
        else:
            #print "Read the following %d bytes from PID %d at 0x%x :" % (read_len, self.pid, read_addr)
            #hexdump(read_buf)
            self.virtual_protect(read_addr, read_len, old_page_perms)
            return read_buf

    def write_memory(self, addr, what_to_write):
        """
            Write the bytes in character array "what" into the process starting
        at the location "address".
        """
        try:
            old_page_perms = self.virtual_protect(addr, len(what_to_write), self.PAGE_READWRITE)
        except:
            pass
        written_count = c_int(0)
        if windll.kernel32.WriteProcessMemory(self.h_process, addr, what_to_write, len(what_to_write), byref(written_count)):
            print "Wrote %d bytes to PID %d at 0x%x" % (written_count.value, self.pid, addr)
        else:
            print FormatError()
            raise "Memory Write Error"
        try:
            self.virtual_protect(read_addr, read_len, old_page_perms)
        except:
            pass
        self.read_memory(addr, len(what_to_write)) #take a peek at changes

       
    def inject_dll(self, path_to_injdll):
        """
            Inject a dll into the process with "Vanilla" dll injection.
        """
        #Justin Seitz's Python code basically ;-)
        path_len  = len(path_to_injdll)
        arg_address = windll.kernel32.VirtualAllocEx(self.h_process, 0, path_len, self.VIRTUAL_MEM, self.PAGE_READWRITE)
        print "VirtualAlloc'd Location: 0x%x" % arg_address
        written = c_int(0)
        windll.kernel32.WriteProcessMemory(self.h_process, arg_address, path_to_injdll, path_len, byref(written))
        h_kernel32 = windll.kernel32.GetModuleHandleA("kernel32.dll")
        h_loadlib  = windll.kernel32.GetProcAddress(h_kernel32,"LoadLibraryA")
        print "LoadLibraryA() is @ : 0x%08x" % h_loadlib
        thread_id = c_ulong(0)
        if not windll.kernel32.CreateRemoteThread(self.h_process,None,0,h_loadlib,arg_address,0,byref(thread_id)):
            print "CreateRemoteThread() failed, thus dLL injection failed. Exiting."
            return
        else:
            print "CreateRemoteThread() was successful, so injection probably was also."
            print "\tInjected dll: '%s' into PID: %d." % (path_to_injdll, self.pid)
        windll.kernel32.CloseHandle(self.h_process) 
