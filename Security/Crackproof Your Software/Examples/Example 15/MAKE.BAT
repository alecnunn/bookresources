@echo off

tasm32 /ml /m3 /z /q detect15.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect15.obj,detect15,, w32.lib, detect15.def

del *.obj
del *.map

pause
cls