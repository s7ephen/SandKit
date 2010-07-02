#include <windows.h>
#include <winnt.h>
#include <stdio.h>
#include <psapi.h>
#include <Winbase.h>
#include <winbase.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <conio.h>


#define HUN 100
#define SPACE " "

//export prototypes
void wtf_error();
char pr1nt(const int *indent);

//------


void wtf_error() {
//This function  a failed attempt at an indentation wrapper for basic output.
	LPVOID lpMsgBuf;
	int err = GetLastError();
	FormatMessage(
			FORMAT_MESSAGE_ALLOCATE_BUFFER |
			FORMAT_MESSAGE_FROM_SYSTEM |
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			err,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL
	);
	printf("\t[!] Error (%d): %s\n", err, (LPCTSTR)lpMsgBuf);
	LocalFree( lpMsgBuf );
};

/*
INCOMPLETE
//input: number of indentations
//output: indentations and line header for easy readable output.
char pr1nt(const int *indent) {
	int *n; char *output[HUN];
	if (indent >= 10) {indent = 10;}; //knock it down some.
	
	for (n=0;n<=indent;n++) {
		sprintf(output, "\t"); //indent one
	};
	if ((*indent % 2) == 0) { //then it is even and we print [+]
		sprintf(output, "[+] ");
	} else { //then it is odd and we print [-]
		sprintf(output, "[-] ");
	};
	return (output);
};
*/
