@echo off

TASM /m9 /n /q detect8.asm
TLINK /3 /x /t detect8.obj

del *.obj

pause
cls