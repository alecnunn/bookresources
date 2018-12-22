@echo off

tasm32 /ml /m3 /z /q detect22.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect22.obj,detect22,, w32.lib, detect22.def

del *.obj
del *.map

pause
cls