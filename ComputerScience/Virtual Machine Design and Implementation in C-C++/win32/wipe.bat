@echo off

cd .\dstruct\hashtbl
del hashtbl.obj
del hashtbl.exe

cd ..\list
del list.obj
del list.exe

cd ..\tree
del tree.obj
del tree.exe

cd ..\..\hasm
del main.obj
del hasm.exe

cd ..\hecvm
del main.obj
del hecvm.exe

cd ..\rawbin
del rawbin.obj
del rawbin.exe

cd ..\util\filedmp
del filedmp.obj
del filedmp.exe

cd ..\numfmt
del numfmt.obj
del numfmt.exe

cd ..\..
