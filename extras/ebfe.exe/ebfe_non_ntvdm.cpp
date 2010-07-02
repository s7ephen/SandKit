/***************************************************************
EBFE the box without the \xeb\fe ntvdm.exe method.
***************************************************************/
#include <stdio.h>
#include <windows.h>
#include <stdio.h>

#define NUMTHREADS 7

//PROTOTYPES
DWORD WINAPI MakeThread( LPVOID lpParam );

DWORD WINAPI dostuff( LPVOID lpParam ) {
        __asm {
                loopr:
                        jmp loopr
        }
        return 0;
};

DWORD WINAPI MakeThread( LPVOID lpParam ) {
    DWORD dwThreadId, dwThrdParam = 1;
    HANDLE hThread;
    hThread = CreateThread(
        NULL,                        // default security attributes
        0,                           // use default stack size
        dostuff,                  // thread function
        &dwThrdParam,                // argument to thread function
                0,//CREATE_SUSPENDED,                           // use default creation flags
        &dwThreadId);                // returns the thread identifier
   // Check the return value for success.
   if (hThread == NULL){
      printf( "CreateThread failed (%d)\n", GetLastError() );
          return 1;
   }
   else{
          return 0;
   }
 }

VOID main( VOID )
{
        int n = 0;
        printf ("EBFE!\n<hit Enter to stop>\n");
        MakeThread(NULL);
        getchar();
}
