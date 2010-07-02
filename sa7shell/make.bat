cls;
rem build the loader executable.
cl /nologo loader.cpp /c
link /nologo /out:loader.exe loader.obj
rem copy loader.exe wrongload.exe

rem build the dll 
cl /nologo pyloader.cpp /c
rem link /dll /def:test.def /ENTRY:dll_entry /SECTION:.text,ERW user32.lib test.obj
rem NOTE!!!! BECAUSE WE use /ENTRY we can run into this error when using
rem functions from the CRT:
rem http://msdn.microsoft.com/en-us/library/9ecfyw6c(VS.80).aspx
rem C Run-Time Error R6030 CRT not initialized
rem After toiling for a day I found the fix here: 
rem http://support.microsoft.com/kb/94248
rem it is to use the ENTRY line below. YES!
rem THIS WILL SOLVE ALL LIBCMT.LIB unresolved symbol errors!!!!
link psapi.lib user32.lib kernel32.lib /SECTION:.text,ERW /ENTRY:_DllMainCRTStartup@12 /dll /def:pyloader.def pyloader.obj
rem link psapi.lib user32.lib kernel32.lib /SECTION:.text,ERW /ENTRY:DllMain /dll /def:pyloader.def pyloader.obj

