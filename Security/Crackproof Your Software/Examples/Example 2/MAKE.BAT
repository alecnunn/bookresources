@echo off

tasm32 /ml /m3 /z /q detect2.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect2.obj,detect2,, w32.lib, detect2.def

del *.obj
del *.map

pause
cls