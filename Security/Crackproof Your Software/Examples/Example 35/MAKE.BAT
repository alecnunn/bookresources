@echo off

tasm32 /ml /m3 /z /q detect35.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect35.obj,detect35,, w32.lib, detect35.def

del *.obj
del *.map

pause
cls