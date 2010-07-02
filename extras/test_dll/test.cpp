// -------------------
/*
    A DLL TO TEST INJECTION
*/
// ------------------

#define _WIN32_WINNT 0x501
#include "sa7_win.h"
#include <psapi.h>
#include <wchar.h>
#include <string.h>
#include <shlwapi.h>
#include <stdio.h>
#include <windows.h>
#include <wincon.h> //AttachConsole

__declspec(dllexport) void console_attach(int pid){
    // THe error I am getting here is that there are _CRT libraries missing
    // I dont know what I am missing from this thing to make it work.
    // TODO: OK so what I need to try here is just resolve ntdll!printf myself
    // and use that.
    if (AttachConsole(pid) != 0){
        MessageBox(NULL, "SUCCESS", "SUCCESS", MB_OK);
        //printf("\nHello from far far away.");
    } else { 
       // printf("\n ERROR: AttachConsole() failed.");
    }; 
};

__declspec(dllexport) void function1(){
    //printf("\nHello, I am function1\n");
};

__declspec(dllexport) void function2(){
	//printf("\nHello, I am function2.\n");
};

//__declspec(dllexport) void dll_entry(){
BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle, 
         IN DWORD     nReason, 
         IN DWORD    Reserved ){
    HANDLE h_self;
    LPTSTR pName;
    char message[256];    
 
    h_self = GetCurrentProcess();
    pName = (LPTSTR)GlobalAlloc(GPTR, 256);
    GetProcessImageFileName(h_self,pName,256);
    MessageBox(NULL, pName, "Injection works. The exe that loaded me is listed below.", MB_OK);
    return TRUE;
};
