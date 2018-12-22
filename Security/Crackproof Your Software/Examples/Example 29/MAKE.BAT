@echo off

tasm32 /ml /m3 /z /q detect29.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect29.obj,detect29,, w32.lib, detect29.def

del *.obj
del *.map

pause
cls