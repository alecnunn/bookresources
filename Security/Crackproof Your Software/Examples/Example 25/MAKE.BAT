@echo off

tasm32 /ml /m3 /z /q detect25.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect25.obj,detect25,, w32.lib, detect25.def

del *.obj
del *.map

pause
cls