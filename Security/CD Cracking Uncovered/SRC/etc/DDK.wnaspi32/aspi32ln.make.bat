@ECHO OFF
IF EXIST aspi32ln.exe DEL aspi32ln.exe

rc.exe aspi32ln.rc 
IF NOT EXIST aspi32ln.RES GOTO ERR

cl.exe aspi32ln.c aspi32ln.RES wnaspi32.lib GDI32.LIB USER32.LIB
IF NOT EXIST aspi32ln.exe GOTO ERR
GOTO END

:ERR
ECHO -ERR: ошибка компиляции!

:END
