@echo off

tasm32 /ml /m3 /z /q detect23.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect23.obj,detect23,, w32.lib, detect23.def

del *.obj
del *.map

pause
cls