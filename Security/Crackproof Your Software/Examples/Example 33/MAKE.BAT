@echo off

tasm32 /ml /m3 /z /q detect33.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect33.obj,detect33,, w32.lib, detect33.def

del *.obj
del *.map

pause
cls