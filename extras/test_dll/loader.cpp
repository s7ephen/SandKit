#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define TARGET_DLL "test.dll"
#define FUNCNAME  "console_attach"

int main(int argc, char **argv) {
	HMODULE htargetdll;
	FARPROC hfunction1, hfunction2;
    FARPROC console_attach;
	
	//Load DLL	
	htargetdll = LoadLibrary(TARGET_DLL);
	if (htargetdll == NULL) {
		printf("\n\t Error loading %s. Quitting", TARGET_DLL);
		return 1;
	};
	
	//Import functions from DLL
	console_attach = (FARPROC)(HANDLE)GetProcAddress(htargetdll,FUNCNAME);
	//Call Imported Functions
    console_attach();
	//Free DLL
	FreeLibrary(htargetdll);
};
