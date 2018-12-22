# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=GAPBILExample - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to GAPBILExample - Win32\
 Debug.
!ENDIF 

!IF "$(CFG)" != "GAPBILExample - Win32 Release" && "$(CFG)" !=\
 "GAPBILExample - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "GAPBILExample.mak" CFG="GAPBILExample - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GAPBILExample - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GAPBILExample - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "GAPBILExample - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "GAPBILExample - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\GAPBILExample.exe"

CLEAN : 
	-@erase ".\Release\GAPBILExample.exe"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\CPBIL.obj"
	-@erase ".\Release\GAPBILExample.pch"
	-@erase ".\Release\GAPBILExampleDoc.obj"
	-@erase ".\Release\GAPBILExample.obj"
	-@erase ".\Release\CGA.obj"
	-@erase ".\Release\CWorld.obj"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\GAPBILExampleView.obj"
	-@erase ".\Release\GAPBILExample.res"
	-@erase ".\Release\CPS.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GAPBILExample.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/GAPBILExample.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GAPBILExample.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/GAPBILExample.pdb" /machine:I386\
 /out:"$(OUTDIR)/GAPBILExample.exe" 
LINK32_OBJS= \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/CPBIL.obj" \
	"$(INTDIR)/GAPBILExampleDoc.obj" \
	"$(INTDIR)/GAPBILExample.obj" \
	"$(INTDIR)/CGA.obj" \
	"$(INTDIR)/CWorld.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/GAPBILExampleView.obj" \
	"$(INTDIR)/CPS.obj" \
	"$(INTDIR)/GAPBILExample.res"

"$(OUTDIR)\GAPBILExample.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GAPBILExample - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\GAPBILExample.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\GAPBILExample.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\GAPBILExample.exe"
	-@erase ".\Debug\GAPBILExampleView.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\CPBIL.obj"
	-@erase ".\Debug\GAPBILExample.obj"
	-@erase ".\Debug\CGA.obj"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\CWorld.obj"
	-@erase ".\Debug\GAPBILExampleDoc.obj"
	-@erase ".\Debug\GAPBILExample.res"
	-@erase ".\Debug\CPS.obj"
	-@erase ".\Debug\GAPBILExample.ilk"
	-@erase ".\Debug\GAPBILExample.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GAPBILExample.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/GAPBILExample.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GAPBILExample.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/GAPBILExample.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/GAPBILExample.exe" 
LINK32_OBJS= \
	"$(INTDIR)/GAPBILExampleView.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/CPBIL.obj" \
	"$(INTDIR)/GAPBILExample.obj" \
	"$(INTDIR)/CGA.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/CWorld.obj" \
	"$(INTDIR)/GAPBILExampleDoc.obj" \
	"$(INTDIR)/CPS.obj" \
	"$(INTDIR)/GAPBILExample.res"

"$(OUTDIR)\GAPBILExample.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "GAPBILExample - Win32 Release"
# Name "GAPBILExample - Win32 Debug"

!IF  "$(CFG)" == "GAPBILExample - Win32 Release"

!ELSEIF  "$(CFG)" == "GAPBILExample - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "GAPBILExample - Win32 Release"

!ELSEIF  "$(CFG)" == "GAPBILExample - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GAPBILExample.cpp
DEP_CPP_GAPBI=\
	".\StdAfx.h"\
	".\GAPBILExample.h"\
	".\MainFrm.h"\
	".\GAPBILExampleDoc.h"\
	".\GAPBILExampleView.h"\
	

"$(INTDIR)\GAPBILExample.obj" : $(SOURCE) $(DEP_CPP_GAPBI) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "GAPBILExample - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GAPBILExample.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GAPBILExample.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GAPBILExample - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GAPBILExample.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GAPBILExample.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\StdAfx.h"\
	".\GAPBILExample.h"\
	".\MainFrm.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GAPBILExampleDoc.cpp
DEP_CPP_GAPBIL=\
	".\StdAfx.h"\
	".\GAPBILExample.h"\
	".\GAPBILExampleDoc.h"\
	".\CWorld.h"\
	".\CPBIL.h"\
	".\CGA.h"\
	".\CPS.h"\
	

"$(INTDIR)\GAPBILExampleDoc.obj" : $(SOURCE) $(DEP_CPP_GAPBIL) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GAPBILExampleView.cpp
DEP_CPP_GAPBILE=\
	".\StdAfx.h"\
	".\GAPBILExample.h"\
	".\GAPBILExampleDoc.h"\
	".\GAPBILExampleView.h"\
	".\CWorld.h"\
	

"$(INTDIR)\GAPBILExampleView.obj" : $(SOURCE) $(DEP_CPP_GAPBILE) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GAPBILExample.rc
DEP_RSC_GAPBILEX=\
	".\res\GAPBILExample.ico"\
	".\res\GAPBILExampleDoc.ico"\
	".\res\GAPBILExample.rc2"\
	

"$(INTDIR)\GAPBILExample.res" : $(SOURCE) $(DEP_RSC_GAPBILEX) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CPBIL.cpp

!IF  "$(CFG)" == "GAPBILExample - Win32 Release"

DEP_CPP_CPBIL=\
	".\StdAfx.h"\
	".\CPBIL.h"\
	
NODEP_CPP_CPBIL=\
	".\dBestFitness"\
	

"$(INTDIR)\CPBIL.obj" : $(SOURCE) $(DEP_CPP_CPBIL) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


!ELSEIF  "$(CFG)" == "GAPBILExample - Win32 Debug"

DEP_CPP_CPBIL=\
	".\StdAfx.h"\
	".\CPBIL.h"\
	

"$(INTDIR)\CPBIL.obj" : $(SOURCE) $(DEP_CPP_CPBIL) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGA.cpp

!IF  "$(CFG)" == "GAPBILExample - Win32 Release"

DEP_CPP_CGA_C=\
	".\StdAfx.h"\
	".\CGA.h"\
	
NODEP_CPP_CGA_C=\
	".\2001"\
	

"$(INTDIR)\CGA.obj" : $(SOURCE) $(DEP_CPP_CGA_C) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


!ELSEIF  "$(CFG)" == "GAPBILExample - Win32 Debug"

DEP_CPP_CGA_C=\
	".\StdAfx.h"\
	".\CGA.h"\
	

"$(INTDIR)\CGA.obj" : $(SOURCE) $(DEP_CPP_CGA_C) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\CWorld.cpp
DEP_CPP_CWORL=\
	".\StdAfx.h"\
	".\CWorld.h"\
	

"$(INTDIR)\CWorld.obj" : $(SOURCE) $(DEP_CPP_CWORL) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CPS.cpp
DEP_CPP_CPS_C=\
	".\StdAfx.h"\
	".\CPS.h"\
	

"$(INTDIR)\CPS.obj" : $(SOURCE) $(DEP_CPP_CPS_C) "$(INTDIR)"\
 "$(INTDIR)\GAPBILExample.pch"


# End Source File
# End Target
# End Project
################################################################################
