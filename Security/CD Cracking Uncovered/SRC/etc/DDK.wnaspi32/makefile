##************************************************************************
##**
##**  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
##**  ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED
##**  TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR
##**  A PARTICULAR PURPOSE.
##**
##**  Copyright (C) 1993 - 1995 Microsoft Corporation. All Rights Reserved.
##**
##**  makefile
##**
##**  HISTORY:
##**	 06/01/96		created.
##**
##************************************************************************

BUILD_TYPE=BLOCK
##!INCLUDE $(BASEDIR)\inc\win_me\master.mk

!if "$(DEBUG)" == "NO" 
OUTDIR=obj
INTDIR=obj
DEBUGOP="NDEBUG"
CPP_PROJ=/nologo /Zp1 /W3 /GX /YX /O2 /D "NDEBUG" /D "_WINDOWS" /D "WIN32" /D\
 "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"aspi32ln.pch" /Fo$(INTDIR)/ /c 
LINK32_FLAGS=kernel32.lib user32.lib gdi32.lib\
 comdlg32.lib advapi32.lib shell32.lib\
 /NOLOGO /SUBSYSTEM:windows /INCREMENTAL:no\
 /PDB:$(OUTDIR)/"aspi32ln.pdb" /MACHINE:I386 /OUT:$(OUTDIR)/"aspi32ln.exe" 
!ELSE
OUTDIR=obj
INTDIR=obj
DEBUGOP="_DEBUG"
CPP_PROJ=/nologo /Zp1 /W3 /GX /Zi /YX /Od /D "_DEBUG" /D "_WINDOWS" /D "WIN32"\
 /D "_MBCS" /FR$(INTDIR)/ /Fp$(OUTDIR)/"aspi32ln.pch" /Fo$(INTDIR)/\
 /Fd$(OUTDIR)/"aspi32ln.pdb" /c 
LINK32_FLAGS=kernel32.lib user32.lib\
 gdi32.lib comdlg32.lib advapi32.lib shell32.lib \
 /NOLOGO /SUBSYSTEM:windows\
 /INCREMENTAL:yes /PDB:$(OUTDIR)/"aspi32ln.pdb" /DEBUG /MACHINE:I386\
 /OUT:$(OUTDIR)/"aspi32ln.exe" 
!ENDIF

ALL : $(OUTDIR)\aspi32ln.exe $(OUTDIR)\aspi32ln.bsc

SOURCE=aspi32ln.C
DEP_ASPI3=aspi32ln.h SCSIDEFS.H wnaspi32.h

$(OUTDIR)\aspi32ln.obj :  $(SOURCE)  $(DEP_ASPI3) $(INTDIR)

RSC=rc.exe
RSC_PROJ=/l 0x1 /fo$(INTDIR)/"aspi32ln.res" /d $(DEBUGOP)

SOURCE=aspi32ln.RC
DEP_ASPI32=aspi32ln.h

$(OUTDIR)\aspi32ln.res :  $(SOURCE)  $(DEP_ASPI32) $(INTDIR)
   $(RSC) $(RSC_PROJ)  $(SOURCE) 

$(OUTDIR) : 
    @if not exist $(OUTDIR)/nul mkdir $(OUTDIR)

MTL=MkTypLib.exe
MTL_PROJ=/nologo /D $(DEBUGOP) /win32 
CPP=cl.exe
CPP_OBJS=$(OUTDIR)/

.c{$(CPP_OBJS)}.obj:
	@echo cl -Fo$@ $<
   $(CPP) @<<
$(CPP_PROJ) $<
<<

BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o$(OUTDIR)/"aspi32ln.bsc" 
BSC32_SBRS=$(OUTDIR)\aspi32ln.sbr

$(OUTDIR)\aspi32ln.bsc : $(OUTDIR)  $(BSC32_SBRS)
!ifndef PASS0ONLY
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<
!endif

DEF_FILE=
LINK32=$(C32_ROOT)\bin\link.exe
LINK32_OBJS=wnaspi32.lib $(OUTDIR)\aspi32ln.obj $(OUTDIR)\aspi32ln.res

$(OUTDIR)\aspi32ln.exe : $(OUTDIR)  $(DEF_FILE) $(LINK32_OBJS)
!ifndef PASS0ONLY
	@echo link -OUT:$@
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<
!endif


clean:
	-@cd $(OUTDIR)
    -@if exist *.obj del *.obj
    -@if exist *.pdb del *.pdb
    -@if exist *.res del *.res
    -@if exist *.ilk del *.ilk
    -@if exist *.exe del *.exe
    -@if exist *.bsc del *.bsc
    -@if exist *.pch del *.pch
    -@if exist *.sbr del *.sbr
	-@cd $(MAKEDIR)
    -@if exist $(OUTDIR)\nul rd $(OUTDIR)
    -@if exist *.bak del *.bak
