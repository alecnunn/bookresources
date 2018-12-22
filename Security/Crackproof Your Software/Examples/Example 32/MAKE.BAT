@echo off

tasm32 /ml /m3 /z /q detect32.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect32.obj,detect32,, w32.lib, detect32.def

del *.obj
del *.map

pause
cls