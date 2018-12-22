@echo off

tasm32 /ml /m3 /z /q detect31.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect31.obj,detect31,, w32.lib, detect31.def

del *.obj
del *.map

pause
cls