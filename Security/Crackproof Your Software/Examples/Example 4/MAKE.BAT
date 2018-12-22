@echo off

tasm32 /ml /m3 /z /q detect4.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect4.obj,detect4,, w32.lib, detect4.def

del *.obj
del *.map

pause
cls