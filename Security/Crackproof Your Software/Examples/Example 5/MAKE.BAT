@echo off

tasm32 /ml /m3 /z /q detect5.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect5.obj,detect5,, w32.lib, detect5.def

del *.obj
del *.map

pause
cls