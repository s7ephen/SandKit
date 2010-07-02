cls;
rem build the loader executable.
cl /nologo loader.cpp /c
link /nologo /out:loader.exe loader.obj

rem build the dll 

cl /nologo globalatom_test.cpp /c
rem link /dll /def:globalatom_test.def /ENTRY:dll_entry /SECTION:.text,ERW user32.lib globalatom_test.obj
link advapi32.lib psapi.lib user32.lib kernel32.lib /SECTION:.text,ERW /ENTRY:_DllMainCRTStartup@12 /dll /def:globalatom_test.def globalatom_test.obj
