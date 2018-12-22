@echo off

tasm32 /ml /m3 /z /q detect21.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect21.obj,detect21,, w32.lib, detect21.def

del *.obj
del *.map

pause
cls