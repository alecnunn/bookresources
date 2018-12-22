@echo off

tasm32 /ml /m3 /z /q detect13.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect13.obj,detect13,, w32.lib, detect13.def

del *.obj
del *.map

pause
cls