@echo off

tasm32 /ml /m3 /z /q detect11.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect11.obj,detect11,, w32.lib, detect11.def

del *.obj
del *.map

pause
cls