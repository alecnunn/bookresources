@echo off

tasm32 /ml /m3 /z /q detect38.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect38.obj,detect38,, w32.lib, detect38.def

del *.obj
del *.map

pause
cls