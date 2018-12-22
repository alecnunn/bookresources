@echo off

tasm32 /ml /m3 /z /q detect36.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect36.obj,detect36,, w32.lib, detect36.def

del *.obj
del *.map

pause
cls