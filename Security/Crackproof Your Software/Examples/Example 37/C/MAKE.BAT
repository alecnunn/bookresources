@echo off

tasm32 /ml /m3 /z /q detect37.asm
tlink32 /Tpe /V4.0 /aa /c /m /ml detect37.obj,detect37,, w32.lib, detect37.def

del *.obj
del *.map

pause
cls