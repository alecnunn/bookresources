@echo off

tasm32 /ml /m3 /z /q detect6.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect6.obj,detect6,, w32.lib, detect6.def

del *.obj
del *.map

pause
cls