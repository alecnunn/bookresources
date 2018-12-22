@echo off

tasm32 /ml /m3 /z /q detect19.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect19.obj,detect19,, w32.lib, detect19.def

del *.obj
del *.map

pause
cls