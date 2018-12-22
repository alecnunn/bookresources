@echo off

tasm32 /ml /m3 /z /q detect7.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect7.obj,detect7,, w32.lib, detect7.def

del *.obj
del *.map

pause
cls