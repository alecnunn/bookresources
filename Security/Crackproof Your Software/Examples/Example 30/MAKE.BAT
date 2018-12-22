@echo off

tasm32 /ml /m3 /z /q detect30.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect30.obj,detect30,, w32.lib, detect30.def

del *.obj
del *.map

pause
cls