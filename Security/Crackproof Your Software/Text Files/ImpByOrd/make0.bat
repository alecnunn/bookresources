@ECHO OFF
LINK -LIB /nologo /MACHINE:IX86 /DEF:subK32.def   /OUT:iKERNEL32sub.lib
LINK -LIB /nologo /MACHINE:IX86 /DEF:subU32.def   /OUT:iUSER32sub.lib
LINK -LIB /nologo /MACHINE:IX86 /DEF:subK32_C.def /OUT:K32forC.lib /IGNORE:4087
LINK -LIB /nologo /MACHINE:IX86 /DEF:subU32_C.def /OUT:U32forC.lib /IGNORE:4087

ML   /c /coff /nologo ImpByOrd.asm
LINK ImpByOrd /nologo /SUBSYSTEM:WINDOWS
DEL  ImpByOrd.obj

LCC  ImpByOrd.c
LINK ImpByOrd /OUT:ImpByOrdC.exe KERNEL32.lib USER32.lib K32forC.lib U32forC.lib /nologo /ENTRY:main /SUBSYSTEM:WINDOWS
DEL  ImpByOrd.obj

DEL  iKERNEL32sub.lib
DEL  iUSER32sub.lib
DEL  iKERNEL32sub.exp
DEL  iUSER32sub.exp
DEL  K32forC.lib
DEL  K32forC.exp
DEL  U32forC.lib
DEL  U32forC.exp