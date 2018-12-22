@echo off

TASM /m9 /n /q detect11.asm
TLINK /3 /x /t detect11.obj

del *.obj

pause
cls