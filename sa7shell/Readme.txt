Sa7Shell
========
Sa7shell is a way to inject the entire python interpreter
into a remote process so that it can be tested from the inside out.
The techniques used for the python interpreter are not unique to python
and can be used for any time you need "interactive" access to code
being injected into a remote process that is not a "console" application.

PyLoader.dll:
"pyloader" is the code  that handles
the asspain of creating a console so that our Python
interactive shell (in the python dll) can execute as
if it were running in a real console. 

IMPORTANT NOTE:
---------------
The Python interpreter I use is python24.dll. It is hardcoded
right now so you gotta change pyloader.dll yourself and recompile. Sorry.


