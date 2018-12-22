@echo off

ECHO.
ECHO  BUILDING HASHTBL++++++++++++++++++
cd .\dstruct\hashtbl
del hashtbl.obj
del hashtbl.exe
nmake /NOLOGO /f hashtbl.mak

ECHO.
ECHO  BUILDING LIST+++++++++++++++++++++
cd ..\list
del list.obj
del list.exe
nmake /NOLOGO /f list.mak

ECHO.
ECHO  BUILDING TREE+++++++++++++++++++++
cd ..\tree
del tree.obj
del tree.exe
nmake /NOLOGO /f tree.mak

PAUSE

ECHO.
ECHO  BUILDING HASM+++++++++++++++++++++
cd ..\..\hasm
del main.obj
del hasm.exe
nmake /NOLOGO /f hasm.mak

ECHO.
ECHO  BUILDING HECVM++++++++++++++++++++
cd ..\hecvm
del main.obj
del hecvm.exe
nmake /NOLOGO /f hecvm.mak

PAUSE

ECHO.
ECHO  BUILDING RAWBIN+++++++++++++++++++
cd ..\rawbin
del rawbin.obj
del rawbin.exe
nmake /NOLOGO /f rawbin.mak

ECHO.
ECHO  BUILDING FILEDMP++++++++++++++++++
cd ..\util\filedmp
del filedmp.obj
del filedmp.exe
nmake /NOLOGO /f filedmp.mak

ECHO.
ECHO  BUILDING NUMFMT+++++++++++++++++++
cd ..\numfmt
del numfmt.obj
del numfmt.exe
nmake /NOLOGO /f numfmt.mak

REM GO BACK HOME+++++++++++++++++++++++
cd ..\..
