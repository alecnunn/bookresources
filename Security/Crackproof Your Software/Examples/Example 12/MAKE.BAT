@echo off

TASM /m9 /n /q detect12.asm
TLINK /3 /x /t detect12.obj

del *.obj

pause
cls