@echo off

tasm32 /ml /m3 /z /q detect18.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect18.obj,detect18,, w32.lib, detect18.def

del *.obj
del *.map

pause
cls