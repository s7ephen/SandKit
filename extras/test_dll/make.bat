cls;
rem build the loader executable.
cl /nologo loader.cpp /c
link /nologo /out:loader.exe loader.obj
rem copy loader.exe wrongload.exe

rem build the dll 
cl /nologo test.cpp /c
rem link /dll /def:test.def /ENTRY:dll_entry /SECTION:.text,ERW user32.lib test.obj
link kernel32.lib psapi.lib user32.lib /SECTION:.text,ERW /ENTRY:DllMain /dll /def:test.def test.obj

