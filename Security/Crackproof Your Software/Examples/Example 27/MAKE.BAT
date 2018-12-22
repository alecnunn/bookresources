@echo off

tasm32 /ml /m3 /z /q detect27.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect27.obj,detect27,, w32.lib, detect27.def

del *.obj
del *.map

pause
cls