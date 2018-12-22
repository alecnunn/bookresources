@echo off

ECHO  MOV HASHTBL++++++++++++++++++
cd .\dstruct\hashtbl
copy hashtbl.exe  ..\..\bin\hashtbl.exe

ECHO  MOV LIST+++++++++++++++++++++
cd ..\list
copy list.exe  ..\..\bin\list.exe

ECHO  MOV TREE+++++++++++++++++++++
cd ..\tree
copy tree.exe  ..\..\bin\tree.exe

ECHO  MOV HASM+++++++++++++++++++++
cd ..\..\hasm
copy hasm.exe  ..\bin\hasm.exe

ECHO  MOV HECVM++++++++++++++++++++
cd ..\hecvm
copy hecvm.exe ..\bin\hecvm.exe

ECHO  MOV RAWBIN+++++++++++++++++++
cd ..\rawbin
copy rawbin.exe ..\bin\rawbin.exe

ECHO  MOV FILEDMP++++++++++++++++++
cd ..\util\filedmp
copy filedmp.exe ..\..\bin\filedmp.exe

ECHO  MOV NUMFMT+++++++++++++++++++
cd ..\numfmt
copy numfmt.exe  ..\..\bin\numfmt.exe

cd ..\..
