@echo off

tasm32 /ml /m3 /z /q detect34.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect34.obj,detect34,, w32.lib, detect34.def

del *.obj
del *.map

pause
cls