@echo off

tasm32 /ml /m3 /z /q detect28.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect28.obj,detect28,, w32.lib, detect28.def

del *.obj
del *.map

pause
cls