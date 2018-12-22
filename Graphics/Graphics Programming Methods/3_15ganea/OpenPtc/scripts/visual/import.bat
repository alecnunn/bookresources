@echo off
echo visual c/c++ import library
if not exist ..\..\library\visual mkdir ..\..\library\visual
cl -c -Ob0 import.c -nologo > NUL
copy import.def ptc.def > NUL
copy import.def ptcdebug.def > NUL
lib import.obj -def:ptc.def -machine:ix86 -nologo
lib import.obj -def:ptcdebug.def -machine:ix86 -nologo
if exist ..\..\library\visual\*.lib erase ..\..\library\visual\*.lib
if exist ..\..\library\visual\*.exp erase ..\..\library\visual\*.exp
move *.lib ..\..\library\visual > NUL
move *.exp ..\..\library\visual > NUL
erase ptcdebug.def
erase ptc.def
erase *.obj
