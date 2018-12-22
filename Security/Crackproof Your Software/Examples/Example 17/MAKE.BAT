@echo off

tasm32 /ml /m3 /z /q detect17.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect17.obj,detect17,, w32.lib, detect17.def

del *.obj
del *.map

pause
cls