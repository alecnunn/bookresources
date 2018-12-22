@echo off

TASM /m9 /n /q detect9.asm
TLINK /3 /x /t detect9.obj

del *.obj

pause
cls