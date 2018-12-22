@echo off

tasm32 /ml /m3 /z /q detect10.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect10.obj,detect10,, w32.lib, detect10.def

del *.obj
del *.map

pause
cls