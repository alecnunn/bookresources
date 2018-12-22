@echo off

tasm32 /ml /m3 /z /q detect1.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect1.obj,detect1,, w32.lib, detect1.def

del *.obj
del *.map

pause
cls