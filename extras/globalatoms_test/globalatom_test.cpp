// -----------------------------------------
/*
    Global Atom Enumeration Proof-of-Concept
*/
// -----------------------------------------

#include "sa7_win.h"
#include <psapi.h>
#include <wchar.h>
#include <string.h>
#include <shlwapi.h>
#include <fcntl.h>     /* for _O_TEXT and _O_BINARY */
#include <windows.h>
#include <aclapi.h>
#include <accctrl.h>
#include <stdio.h>
#include <assert.h>
#define MAX_ATOM_LENGTH 250

/* 
    I couldn't find examples/patterns for how to enumerate
    all Global Atoms, except for this one in VisualFoxPro
    so I ported it to C. It uses the same "descriptor" bruteforcing
    techinque.

#DEFINE MAX_ATOM_LENGTH 250
 
DECLARE INTEGER GlobalGetAtomName IN kernel32;
    INTEGER nAtom, STRING @lpBuffer, INTEGER nSize

CREATE CURSOR cs (atom N(12), atomname C(250))
 
* scan string atoms
FOR nAtom = 0xC000 TO 0xFFFF
    lpBuffer = REPLICATE(Chr(0), MAX_ATOM_LENGTH)
    lnResult = GlobalGetAtomName (nAtom, @lpBuffer, MAX_ATOM_LENGTH)
 
    IF lnResult > 0
        INSERT INTO cs VALUES (;
            nAtom, LEFT(lpBuffer, lnResult))
    ENDIF
ENDFOR
 
SELECT cs
GO TOP
BROW NORMAL NOWAIT

*/

__declspec(dllexport) void enumerate_atoms(){
    LPTSTR lpBuffer;
    int res;
    ATOM i;
    //Windef.h has: typedef WORD ATOM; so ATOMs aint nuffing but numbers.
     
    lpBuffer = (LPTSTR)GlobalAlloc(0x0042,MAX_ATOM_LENGTH);
    for (i = 0x0000; i < 0xFFFF; i++){ // I dont know why this starts at 0xC0000
        res = GlobalGetAtomName(i,lpBuffer, MAX_ATOM_LENGTH);
        if (res > 0)
            printf("Found a GlobalAtom at ATOM: %d named: %s\n", i, lpBuffer);
    }
}


//The idea for these server and client exports is to just put all the
//functionality into the DLL and then have two separate executables that each
//access the individual functions.
__declspec(dllexport) void poc_client(){
//This stub of code is responsible for accessing the executable character buffer
//created by the Poc_server
}
__declspec(dllexport) void poc_server(){
//This stub of code is responsible for creating a GlobalAtom with executable
//character buffer in it.
}

//__declspec(dllexport) void dll_entry(){
BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle, 
         IN DWORD     nReason, 
         IN LPVOID    Reserved ){

    //I need to add the if statement that checks whether this
    // is the load or the unload of the module. But I am lazy
    // This will happen later.
    //printf("\nC Module: '%s' loaded.", module_name);

    return TRUE;
};
