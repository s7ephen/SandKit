from cmd import *
import os
import code
import sys
import pprint as pp
import struct
import threading
import md5
import ctypes
#--- Below are .py modules from the Sandkit bundle
#    so path matters.
import litedbg
import bincompare

class BaseUI(Cmd):
    """

        The base User Interface object.

    """
    path =[] #this is how we fake the "path" of commands.
    name = ""

    def __init__(self):
        Cmd.__init__(self)
        
    def make_prompt(self, name=""):
        test_str = self.get_prompt()
        if test_str.endswith(name):
            test_str += ">> "
            return(test_str)
        #the above is a little hack to test if the path
        #is already set for us, incase this object instance
        #is actually getting reused under the hood.
        self.path.append(name)
        tmp_name = ""
        tmp_name = self.get_prompt()
        tmp_name += ">> "
        return(tmp_name)

    def get_prompt(self):
        tmp_name = ""
        for x in self.path: #iterate through object heirarchy
            tmp_name += (x)
        return tmp_name

    def do_help(self, args):
        """
           Getting help on "help" is kinda tautological dont you think?
        """
        #The only reason to define this method is for the help text in the
        #docstring   
        Cmd.do_help(self, args)

    def do_hist(self, args):
        """

            Display command history. 

        """
#        n = 0
#        for i in self._hist:
#            print "%d: %s" % (n, i)
#            n+=1 
        pp.pprint(self._hist)

    def emptyline(self):
        """
            Do nothing on empty input line
        """
        pass

    def preloop(self):
        """
            Initialization before prompting user for commands.
            Despite the claims in the Cmd documentaion, Cmd.preloop() is not a
            stub.
        """
        Cmd.preloop(self)   ## sets up command completion
        self._hist    = []      ## No history yet
        self._locals  = {}      ## Initialize execution namespace for user
        self._globals = {}

    def postloop(self):
        """
            Take care of any unfinished business.
            Despite the claims in the Cmd documentaion, Cmd.postloop() is not a
            stub.
        """
        Cmd.postloop(self)   ## Clean up command completion
        print "\nExiting..."

    def precmd(self, line):
        """ 
            This method is called after the line has been input but before
            it has been interpreted. If you want to modifdy the input line
            before execution (for example, variable substitution) do it here.

        """
        self._hist+=[line.strip()]
        return line

    def postcmd(self, stop, line):
        """
            If you want to stop the console, return something that evaluates to
            true. If you want to do some post command processing, do it here.

        """
        return stop

    def default(self, line):
        """
            Called on an input line when the command prefix is not recognized.
            In that case we execute the line as Python code.

        """
        print "\nBad command or filename.'"

    def do_exit(self, args):
        """
            Exit this level of the CLI. If you are in a submenu
            it will "cd .." to the parent menu.
            If you are in the main menu, it will exit.

        """
        return -1

    do_EOF = do_exit

    def checkargs(self, args, num_args=None):
        """
        A utility function to split up the args.
        Also check check the number of args against
        a number of arguments
        """
        splitted_args = args[0].split(' ')
        if splitted_args.__contains__(''):
            splitted_args.remove('')
        if num_args == None:
            return splitted_args
        if (len(splitted_args) < num_args):
            print "Incorrect number of arguments."
            return 
        else:
            return splitted_args

class MasterUI(BaseUI):
    """ Master UI """

    def __init__(self, prompt, intro):
        BaseUI.__init__(self)
        self.prompt = self.make_prompt(prompt)
        self.intro = intro
        self.doc_header = "...oooOOO SandKit Command  OOOooo..."
        self.doc_header+="\n (for help, type: help <command>)"
        self.undoc_header = ""
        self.misc_header = ""
        self.ruler = " "
        self.pyloader = "c:\\pyloader.dll"

    def do_pythonshell(self, *args):
        """

        Use this to drop BACK to an interactive python shell.
        This can be used to then enter python code or import python
        modules as you would with the normal python interactive shell.

        """
        banner = "\n *** Welcome to SandKit Interactive Python Console ***\n"
        banner += "Break out with CTRL-Z."
        code.interact(banner,None, {})
    
    def do_readmem(self, *args):
        """

        Read and display memory from a process.
        The SandKit "ps" command will list pids/processes.

        Usage:
            readmem <pid> <address> <length in bytes>

        Example:
            readmem 2764 0x7c900000 20
        """
        args = self.checkargs(args, 3)
        if args == None:
            return
        if args[1].__contains__("0x"): #python fucking sucks sometimes
            args[1] = int(args[1],16)  #so we have to do this
        dbg1 = litedbg.LiteDbg(int(args[0]))
        buf = dbg1.read_memory(int(args[1]), int(args[2]))
        litedbg.hexdump(buf)
        del(dbg1)

    def do_writemem(self, *args):
        """

        Write a character array to a location in a process's memory. 
            The SandKit "ps" command will list pids/processes.
            The SandKit "readmem" command can be used to display the memory region
            before and after.

        Usage:
            writemem <pid> <address> <string to write to memory>

        Example:
            writemem 2764 0x7c900000 "\\x90\\x90\\x90\\x90"
            or 
            writemem 2764 0x7c900000 "This is a test."

        Note: Please do "all or nothing". In other words, please
            don't mix and match excaped bytes with non-escaped
            bytes in the string like: "\\x90ABCDEF\\x90"
        """
        args = self.checkargs(args, 3)
        if args == None:
            return
        if args[2].__contains__('\\x'): #handle escaped bytes
            args[2] = args[2].replace('"',''); args[2] = args[2].replace('\'','')
            args[2] = args[2].replace('\\x',' ').split(' ')
            i = 0
            for x in args[2]:
                if x == '':
                    args[2].__delitem__(i)
                i+=1
            args[2] = ''.join([struct.pack("B",int(x,16)) for x in args[2]])
        #print repr(args)
        #print len(args[2])
        if args[1].__contains__("0x"): #python fucking sucks sometimes
            args[1] = int(args[1],16)  #so we have to do this
        # If you are wondering why all the attaches and reattaches
        # I just want to make sure that everything is "fresh" 
        # I was encoutering bugs where not doing so would cause problems.
        dbg1 = litedbg.LiteDbg(int(args[0]))
        buf = dbg1.read_memory(int(args[1]), len(args[2]))
        print "----------------------"
        print "MEMORY BEFORE CHANGES:"
        print "----------------------"
        litedbg.hexdump(buf)
        del(dbg1)
        dbg2 = litedbg.LiteDbg(int(args[0]))
        dbg2.write_memory(int(args[1]), args[2])
        del(dbg2)
        dbg3 = litedbg.LiteDbg(int(args[0]))
        buf = dbg3.read_memory(int(args[1]), len(args[2]))
        print "---------------------"
        print "MEMORY AFTER CHANGES:"
        print "---------------------"
        litedbg.hexdump(buf)
        del(dbg3) 

    def do_dumpmem(self, *args):
        """
        Read memory from a process and write it to a file.

        Usage:
            dumpmem <pid> <address> <length in bytes> <file to dump to>

        """
        args = self.checkargs(args, 4)
        if args == None:
            return
        if args[1].__contains__("0x"): #python fucking sucks 
            args[1] = int(args[1],16)  #so we have to do this
        else:
            args[1] = int(args[1])
        try:
            f_h = open(args[3],'wb')
        except:
            print "Error opening file for writing. Aborting."
            return
        dbg1 = litedbg.LiteDbg(int(args[0]))
        buf = dbg1.read_memory(int(args[1]), int(args[2]))
        f_h.write(buf)
        print "Wrote %d bytes to file '%s'." % (len(buf), args[3])
        del(dbg1)
        f_h.close()

    def do_copymem(self, *args):
        """

        Copy memory regions from one process into another.

        Usage:
            copymem <source-pid> <address> <length in bytes> <target-pid> <address>

        Example:
            copymem 3000 0x7c885000 5 3001 0x7c885000

        """
        args = self.checkargs(args, 5)
        if args == None:
            return
        if args[1].__contains__("0x"): #python fucking sucks sometimes
            args[1] = int(args[1],16)  #so we have to do this
        if args[4].__contains__("0x"): #python fucking sucks sometimes
            args[4] = int(args[4],16)  #so we have to do this
        dbg1 = litedbg.LiteDbg(int(args[0]))
        dbg2 = litedbg.LiteDbg(int(args[3]))
        try:
            buf1 = dbg1.read_memory(int(args[1]), int(args[2]))
        except:
            print "Memory read error."
            return
        try:
            buf2 = dbg2.read_memory(int(args[1]), int(args[2]))
        except:
            print "Memory read error."
            return
        print "------------------------------------------"
        print "MEMORY FROM SOURCE PID: %d @0x%08X" % (dbg1.pid, int(args[1])) 
        print "------------------------------------------"
        litedbg.hexdump(buf1)
        print "----------------------------------------------"
        print "MEMORY FROM DEST PID %d @0x%08X (before)" % (dbg2.pid, int(args[4])) 
        print "----------------------------------------------"
        litedbg.hexdump(buf2)
        try:
            dbg2.write_memory(int(args[4]), buf1)
        except:
            print "Memory write error."
            return
        dbg3 = litedbg.LiteDbg(int(args[3]))
        buf2 = dbg3.read_memory(int(args[4]), int(args[2]))
        print "----------------------------------------------"
        print "MEMORY FROM DEST PID %d @0x%08X (after)" % (dbg2.pid, int(args[4])) 
        print "----------------------------------------------"
        litedbg.hexdump(buf2)
        del(dbg1)
        del(dbg2)
        del(dbg3) 

    def do_hook_fix(self, *args):
        """

        Chrome specific hook unhooker ;-)

        Usage:
            hook_fix

        """
        print "Not Yet Implemented in Public Sandkit."

    def do_memdiff(self, *args):
        """

        Compare two regions of memory in two different processes
        and report where these regions of memory differ.

        Usage:
            memdiff <pid-one> <address> <length in bytes> <pid-two> <address> 

        Example (kernel32 is at 0x7c800000):
            memdiff 2288 0x7c885000 20 1840 0x7c885000

        """
        args = self.checkargs(args, 5)
        if args == None:
            return
        if args[1].__contains__("0x"): #python fucking sucks 
            args[1] = int(args[1],16)  #so we have to do this
        else:
            args[1] = int(args[1])
        if args[4].__contains__("0x"): #python fucking sucks 
            args[4] = int(args[4],16)  #so we have to do this
        else:
            args[4] = int(args[4])
        dbg1 = litedbg.LiteDbg(int(args[0]))
        buf1 = dbg1.read_memory(int(args[1]), int(args[2]))
        dbg2 = litedbg.LiteDbg(int(args[3]))
        buf2 = dbg2.read_memory(int(args[4]), int(args[2]))
        bincompare.compare(buf1, buf2, 0)
        del(dbg1)
        del(dbg2)

    def do_ps(self, *args):
        """
        List all processes on the system by name and pid.
        Or get a list of process pid's of processes whose name match 
        the name you specify.
        
        Usage:
            ps 
                -- or --
            ps <case sensitive string>

        Example:
            ps Explorer

        """
        args = self.checkargs(args)
        #print repr(args)
        if len(args) == 0:
            litedbg.ps()
        else:
            print "Pids of processes with names matching '%s':" % (args[0])
            litedbg.ps(str(args[0]))
        
    def do_injectdll(self, *args):
        """

        Using vanilla DLL injection, inject a dll into a remote process.
        
        Usage:
            injectdll <pid> <path-to-dll-to-inject>

            Note: extras/test_dll in SandKit bundle contains a simple dll
                to test your injections.

        """
        args = self.checkargs(args, 2)
        dbg = litedbg.LiteDbg(int(args[0]))
        dbg.inject_dll(args[1])
        del(dbg)

    def do_injectdll2(self, *args):
        """

        Using more advanced DLL injection, inject a dll into a remote process.
        
        Usage:
            injectdll <pid> <path-to-dll-to-inject>

        """
        print "Not Yet Implemented in publicly released SandKit." 

    def do_sa7shell(self, *args):
        """

        Inject python shell in target process.

        Usage:
            sa7shell <pid> 

        """
        err_msg = """
Could not find c:\\pyloader.dll. Be sure that you have copied
it from the "sa7shell" directory in the SandKit bundle to c:\\. Alternatively if you
don't want it there, just use the "injectdll" command in SandKit 
shell to specify an alternate path.
"""
        print repr(args)
        if os.path.exists(self.pyloader):
            dbg = litedbg.LiteDbg(int(args[0]))
            dbg.inject_dll(self.pyloader)
        else:
            print err_msg

#if __name__ == "__main__":
welcome = """
    
    ...oooOOO Welcome to OOOooo...

               SandKit 

    ...oooOOOOOOOOOOOOOOOOOOooo...

"""
MasterUI("SandKit", welcome).cmdloop()
