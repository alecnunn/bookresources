@echo off

tasm32 /ml /m3 /z /q detect16.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect16.obj,detect16,, w32.lib, detect16.def

del *.obj
del *.map

pause
cls