cls;
rem build the loader executable.
cl /nologo loader.cpp /c
link /nologo /out:loader.exe loader.obj

rem build the dll 

cl /nologo token_sniper.cpp /c
rem link /dll /def:token_sniper.def /ENTRY:dll_entry /SECTION:.text,ERW user32.lib token_sniper.obj
link advapi32.lib psapi.lib user32.lib kernel32.lib /SECTION:.text,ERW /ENTRY:_DllMainCRTStartup@12 /dll /def:token_sniper.def token_sniper.obj
