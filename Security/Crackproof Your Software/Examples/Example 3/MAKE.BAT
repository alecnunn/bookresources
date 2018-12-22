@echo off

tasm32 /ml /m3 /z /q detect3.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect3.obj,detect3,, w32.lib, detect3.def

del *.obj
del *.map

pause
cls