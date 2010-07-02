#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#define TARGET_DLL "pyloader.dll"
#define FUNCTION_IN_TARGET_DLL "function1"
#define FUNCTION2_IN_TARGET_DLL "function2"


int main(int argc, char **argv) {
	HMODULE htargetdll;
	FARPROC hfunction1, hfunction2;
	
	//Load DLL	
	htargetdll = LoadLibrary(TARGET_DLL);
	if (htargetdll == NULL) {
		printf("\n\t Error loading %s. Quitting", TARGET_DLL);
		return 1;
	};
	
	//Import functions from DLL
	hfunction1 = (FARPROC)(HANDLE)GetProcAddress(htargetdll,FUNCTION_IN_TARGET_DLL);
	hfunction2 = (FARPROC)(HANDLE)GetProcAddress(htargetdll,FUNCTION2_IN_TARGET_DLL);

	//Call Imported Functions
//	hfunction1();
//	hfunction2();

	//Free DLL
	FreeLibrary(htargetdll);
};
