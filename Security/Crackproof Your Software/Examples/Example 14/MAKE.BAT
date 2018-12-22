@echo off

tasm32 /ml /m3 /z /q detect14.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect14.obj,detect14,, w32.lib, detect14.def

del *.obj
del *.map

pause
cls