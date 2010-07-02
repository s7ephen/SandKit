// -------------------
/*
    A DLL to handle all the "setup" to make is possible
    to inject Python24 into any process on a windows system.
*/
// ------------------

#define _WIN32_WINNT 0x0501
#include "sa7_win.h"

#include <Psapi.h>
#include <wchar.h>
#include <string.h>
#include <shlwapi.h>
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <fcntl.h>
#include <io.h>

__declspec(dllexport) void function1(){
    //printf("\nHello, I am function1\n");
};

__declspec(dllexport) void function2(){
	//printf("\nHello, I am function2.\n");
};

// *****************************************************************
/* Ok this is truly retarded but the following is necessary to "trick" the
 * linker into creating a symbol "main" needed by a libcmt function. Without it you
 * get the following compiler errors. it's almost as if somewhere in the cmt
 * the name of ENTRY is statically coded to be "main" or something:

 LIBCMT.lib(crt0.obj) : error LNK2019: unresolved external symbol _main
 referenced in function ___tmainCRTStartup pyloader.dll : fatal error LNK1120: 1 unresolved externals
*/
void main(){MessageBox(NULL, "Main()", "!", MB_OK);};
// *****************************************************************

typedef void (*PInteractiveLoop)(HANDLE, const char *);
typedef void (*PSimpleString)(const char *);
//BOOL WINAPI _CRT_INIT(HINSTANCE hDllHandle, DWORD nReason, LPVOID Reserved);

//__declspec(dllexport) void dll_entry(){
BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle, IN DWORD nReason, IN LPVOID Reserved ){
    HANDLE h_self, consoletest;
    LPTSTR pName;
    char message[256];
    char stdin_testbuf[50];
    LPTSTR thang;
    HMODULE h_python;
    HANDLE h_stdin, h_stdout, h_stderr;
    FARPROC pyInit, pyFinal;
    PInteractiveLoop pyInteract;
    PSimpleString pyRunString;
    int hConHandle;
    //long lStdHandle = (long)INVALID_HANDLE_VALUE;
    long lStdHandle;
    FILE *fp;

//    _CRT_INIT(hDllHandle, nReason, Reserved); // see: http://support.microsoft.com/kb/94248
//This is necessary because we use /ENTRY explicitly and inject
    if (nReason != DLL_PROCESS_ATTACH)
        return TRUE; //We are not being loaded we are being unloaded or something else.
    h_self = GetCurrentProcess();
    pName = (LPTSTR)GlobalAlloc(GPTR, 256);
    GetProcessImageFileName(h_self,pName,256);
    /* DETECT PRESENCE OF CONSOLE?

        The test below does not actually work because GUI apps (like notepad
        ) show that the application does have a STDOUT handle still defined
        even though there is not an active Console window.... need a better way
        of testing CONSOLE mode...like Pascal's isConsole() API call.

    consoletest = GetStdHandle(STD_OUTPUT_HANDLE);
    if (consoletest != INVALID_HANDLE_VALUE) {
       MessageBox(NULL, "There appears to already be a console!", "!", MB_OK); 
    } else {
        MessageBox(NULL, "There is not already a console!", "!", MB_OK);
    }

    */


    //Ok, this is a lame trick but it seems to work! From testing, it looks like
    //GetConsoleTitle() is a cheap way to detect whether an app even has a
    //Console created, it also seems to adequately test whether an app even has
    //console capabilities. I tested this by injecting into a bunch of different apps
    //and it seems to be reliable.
    thang = (LPTSTR)GlobalAlloc(GMEM_ZEROINIT, 2000);
    if (GetConsoleTitle(thang,1999) == 0) //Console window does not exist
                                           //so we have to create one.
        MessageBox(NULL, "No Console Window exists. Creating one.", "!", MB_OK);
    else
        MessageBox(NULL, pName, "A Console already exists. ", MB_OK);

    if (!AllocConsole()){
        MessageBox(NULL, "Can not AllocConsole()!", "!", MB_OK);
//            return TRUE;
    } else {
        MessageBox(NULL, "AllocConsole() successful!", "!", MB_OK);
        SetConsoleTitle("Sa7oriShell running in yer processes...");
    }


    //***********************************************
    // BEGIN HANDLE SHENNANEGANS
    //   The idea for all this handle fanciness below on STDIN/STDOUT/STDERR
    //   came from this: http://support.microsoft.com/kb/105305
    //***********************************************
    // redirect unbuffered STDOUT to the console
    lStdHandle = (long)GetStdHandle(STD_OUTPUT_HANDLE);
    if (lStdHandle == (long)INVALID_HANDLE_VALUE)
        MessageBox(NULL, "Could not get STD_OUTPUT_HANDLE", "!", MB_OK);
    //The next line causes process to exit with no exceptions when injected
    //into remote process.
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);// _O_TEXT defined in
                                                       //#include <fcntl.h> and _open_osfhandle in io.h
    if (hConHandle == -1)
        MessageBox(NULL, "Could not open STD_INPUT_HANDLE", "!", MB_OK);
    fp = _fdopen( hConHandle, "w" );
    *stdout = *fp;
    setvbuf( stdout, NULL, _IONBF, 0 );

    // redirect unbuffered STDIN to the console
    lStdHandle = (long)GetStdHandle(STD_INPUT_HANDLE);
    if (lStdHandle == (long)INVALID_HANDLE_VALUE)
        MessageBox(NULL, "Could not get STD_INPUT_HANDLE", "!", MB_OK);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    if (hConHandle == -1)
        MessageBox(NULL, "Could not open STD_INPUT_HANDLE", "!", MB_OK);
    fp = _fdopen( hConHandle, "r" );
    *stdin = *fp;
    setvbuf( stdin, NULL, _IONBF, 0 );

    // redirect unbuffered STDERR to the console
    lStdHandle = (long)GetStdHandle(STD_ERROR_HANDLE);
    if (lStdHandle == (long)INVALID_HANDLE_VALUE)
        MessageBox(NULL, "Could not get STD_ERROR_HANDLE", "!", MB_OK);
    hConHandle = _open_osfhandle(lStdHandle, _O_TEXT);
    if (hConHandle == -1)
        MessageBox(NULL, "Could not open STD_ERROR_HANDLE", "!", MB_OK);
    fp = _fdopen( hConHandle, "w" );    
    *stderr = *fp;
    setvbuf( stderr, NULL, _IONBF, 0 );
    //***********************************************
    // END HANDLE SHENNANEGANS
    //***********************************************

    printf("\nIf you can read this, a Console has been successfully allocated!\n");
    printf("\nTesting STDIN: type some text then hit <ENTER>\n");
    if (fgets(stdin_testbuf, sizeof(stdin_testbuf)-1, stdin)!= NULL)
        printf("You entered: \"%s\"\n", stdin_testbuf);
    else
        printf("Did not receive any input.\n"); 
    
    h_python = LoadLibraryA("python24.dll"); //This has to happen AFTER the STD fanciness above because
                                            //when the python library loads his CRT initializes. I spent
                                            // DAYS debugging this problem trying to figure out why python
                                            // was using a DIFFERENT set of STD handles than the ones I created
                                            // moving this LoadLibrary call to AFTER all the handle swapping worked!
                                            // because then the python dll
                                            // inherits the "correct" set of
                                            // handles.
                                            // 17Feb2010 20:12 at kitchen table!
    if (h_python == NULL){
        MessageBox(NULL, "Unable to load python24.dll", "!", MB_OK);
    } else {
        MessageBox(NULL, "Successfully loaded python24.dll", "!", MB_OK);

        pyInit = (FARPROC)(HANDLE)GetProcAddress(h_python, "Py_Initialize");
        pyFinal = (FARPROC)(HANDLE)GetProcAddress(h_python, "Py_Finalize");
        pyInteract = (PInteractiveLoop)GetProcAddress(h_python,"PyRun_InteractiveLoop");
        pyRunString = (PSimpleString)GetProcAddress(h_python, "PyRun_SimpleString");
        printf("Py_Initialize() is @ %08x\n", pyInit);
        printf("Py_Finalize() is @ %08x\n", pyFinal);
        printf("PyRun_InteractiveLoop() is @ %08x\n", pyInteract);
        printf("PyRun_SimpleString() is @ %08x\n", pyRunString);
        if ((pyInit != NULL) && (pyInteract != NULL) && (pyFinal!= NULL)){
            printf("Jumping into the python dll.");
            pyInit();
            //pyRunString("fh=open(\"c:\\sa7.txt\",'w');fh.write(\"PyRun_SimpleString() works!\");fh.close();");
            //printf("\nI just PyRun_SimplyString()ed a string that 'touch'ed c:\\sa7.txt!\n check that it is there.\n");
            pyRunString("print '\\nThis is a test print() from inside python.'");
            pyRunString("import sys;\nsys.ps1=\"sa7>>>\"");
            printf("\nAttempting to enter interactive mode of python...\n");
            //pyRunString("import sys,code,ctypes");
            //pyRunString("sys.stdin = open(\"CONIN$\",\"r\")");
            //pyRunString("ctypes.windll.kernel32.AllocConsole()");
            //pyRunString("sys.stdout = open('CONOUT$', 'w')\n");
            //pyRunString("sys.stderr = open('CONOUT$', 'w')\n");
            //pyRunString("code.interact(locals())\n");
            pyInteract(stdin, "<stdin>");
            //printf ("PyRun_InteractiveLoop() return: %d\n",ret);
            //pyFinal();
        } else {
            MessageBox(NULL, "Was unable to resolve critical functions in python dll.", "!", MB_OK);
            return TRUE;
        }
        //__asm int 3;
        //return TRUE;
    }
    return TRUE;
};
