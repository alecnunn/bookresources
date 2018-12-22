@echo off

ECHO test1.asm
..\..\bin\hasm test1.asm

ECHO fileIO.asm
..\..\bin\hasm fileIO.asm

ECHO filemgmt.asm
..\..\bin\hasm filemgmt.asm

ECHO procmgmt.asm
..\..\bin\hasm procmgmt.asm

ECHO timedate.asm
..\..\bin\hasm timedate.asm

ECHO cmdline.asm
..\..\bin\hasm cmdline.asm

ECHO memtest.asm
..\..\bin\hasm memtest.asm

ECHO alloc.asm 
..\..\bin\hasm alloc.asm

ECHO native.asm
..\..\bin\hasm native.asm

ECHO ipc1.asm
..\..\bin\hasm ipc1.asm

ECHO ipc2.asm
..\..\bin\hasm ipc2.asm

ECHO ipc3.asm
..\..\bin\hasm ipc3.asm

ECHO ipc4.asm
..\..\bin\hasm ipc4.asm

DEL *.TMP
