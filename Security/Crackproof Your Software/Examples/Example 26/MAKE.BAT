@echo off

tasm32 /ml /m3 /z /q detect26.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect26.obj,detect26,, w32.lib, detect26.def

del *.obj
del *.map

pause
cls