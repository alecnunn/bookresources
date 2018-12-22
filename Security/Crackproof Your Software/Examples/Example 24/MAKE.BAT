@echo off

tasm32 /ml /m3 /z /q detect24.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect24.obj,detect24,, w32.lib, detect24.def

del *.obj
del *.map

pause
cls