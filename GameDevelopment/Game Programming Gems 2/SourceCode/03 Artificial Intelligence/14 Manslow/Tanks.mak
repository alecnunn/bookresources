# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Tanks - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Tanks - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Tanks - Win32 Release" && "$(CFG)" != "Tanks - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "Tanks.mak" CFG="Tanks - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Tanks - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Tanks - Win32 Debug" (based on "Win32 (x86) Application")
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
CPP=cl.exe
RSC=rc.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Tanks - Win32 Release"

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

ALL : "$(OUTDIR)\Tanks.exe"

CLEAN : 
	-@erase ".\Release\Tanks.exe"
	-@erase ".\Release\Tanks.obj"
	-@erase ".\Release\Tanks.pch"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\TanksDoc.obj"
	-@erase ".\Release\TanksView.obj"
	-@erase ".\Release\Tanks.res"
	-@erase ".\Release\CWorld.obj"
	-@erase ".\Release\CProjectile.obj"
	-@erase ".\Release\CTank.obj"
	-@erase ".\Release\CMLP.obj"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tanks.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/Tanks.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tanks.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/Tanks.pdb" /machine:I386 /out:"$(OUTDIR)/Tanks.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Tanks.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/TanksDoc.obj" \
	"$(INTDIR)/TanksView.obj" \
	"$(INTDIR)/CWorld.obj" \
	"$(INTDIR)/CProjectile.obj" \
	"$(INTDIR)/CTank.obj" \
	"$(INTDIR)/CMLP.obj" \
	"$(INTDIR)/Tanks.res"

"$(OUTDIR)\Tanks.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Tanks - Win32 Debug"

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

ALL : "$(OUTDIR)\Tanks.exe"

CLEAN : 
	-@erase ".\Debug\Tanks.exe"
	-@erase ".\Debug\Tanks.obj"
	-@erase ".\Debug\Tanks.pch"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\TanksDoc.obj"
	-@erase ".\Debug\TanksView.obj"
	-@erase ".\Debug\Tanks.res"
	-@erase ".\Debug\CWorld.obj"
	-@erase ".\Debug\CProjectile.obj"
	-@erase ".\Debug\CTank.obj"
	-@erase ".\Debug\CMLP.obj"
	-@erase ".\Debug\Tanks.ilk"
	-@erase ".\Debug\Tanks.pdb"
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\vc40.idb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tanks.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/Tanks.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/Tanks.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/Tanks.pdb" /debug /machine:I386 /out:"$(OUTDIR)/Tanks.exe" 
LINK32_OBJS= \
	"$(INTDIR)/Tanks.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/TanksDoc.obj" \
	"$(INTDIR)/TanksView.obj" \
	"$(INTDIR)/CWorld.obj" \
	"$(INTDIR)/CProjectile.obj" \
	"$(INTDIR)/CTank.obj" \
	"$(INTDIR)/CMLP.obj" \
	"$(INTDIR)/Tanks.res"

"$(OUTDIR)\Tanks.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "Tanks - Win32 Release"
# Name "Tanks - Win32 Debug"

!IF  "$(CFG)" == "Tanks - Win32 Release"

!ELSEIF  "$(CFG)" == "Tanks - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Tanks - Win32 Release"

!ELSEIF  "$(CFG)" == "Tanks - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Tanks.cpp
DEP_CPP_TANKS=\
	".\StdAfx.h"\
	".\Tanks.h"\
	".\MainFrm.h"\
	".\TanksDoc.h"\
	".\TanksView.h"\
	

"$(INTDIR)\Tanks.obj" : $(SOURCE) $(DEP_CPP_TANKS) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Tanks - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tanks.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c\
 $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tanks.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Tanks - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/Tanks.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\Tanks.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\StdAfx.h"\
	".\Tanks.h"\
	".\MainFrm.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TanksDoc.cpp
DEP_CPP_TANKSD=\
	".\StdAfx.h"\
	".\Tanks.h"\
	".\TanksDoc.h"\
	".\CWorld.h"\
	".\CTank.h"\
	".\CProjectile.h"\
	".\CMLP.h"\
	

"$(INTDIR)\TanksDoc.obj" : $(SOURCE) $(DEP_CPP_TANKSD) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TanksView.cpp
DEP_CPP_TANKSV=\
	".\StdAfx.h"\
	".\Tanks.h"\
	".\TanksDoc.h"\
	".\TanksView.h"\
	".\CWorld.h"\
	".\CProjectile.h"\
	".\CTank.h"\
	".\CMLP.h"\
	

"$(INTDIR)\TanksView.obj" : $(SOURCE) $(DEP_CPP_TANKSV) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Tanks.rc
DEP_RSC_TANKS_=\
	".\res\Tanks.ico"\
	".\res\TanksDoc.ico"\
	".\res\Tanks.rc2"\
	

"$(INTDIR)\Tanks.res" : $(SOURCE) $(DEP_RSC_TANKS_) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CWorld.cpp
DEP_CPP_CWORL=\
	".\StdAfx.h"\
	".\CWorld.h"\
	".\CTank.h"\
	".\CProjectile.h"\
	

"$(INTDIR)\CWorld.obj" : $(SOURCE) $(DEP_CPP_CWORL) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CProjectile.cpp
DEP_CPP_CPROJ=\
	".\StdAfx.h"\
	".\CProjectile.h"\
	

"$(INTDIR)\CProjectile.obj" : $(SOURCE) $(DEP_CPP_CPROJ) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CTank.cpp
DEP_CPP_CTANK=\
	".\StdAfx.h"\
	".\CTank.h"\
	".\CProjectile.h"\
	

"$(INTDIR)\CTank.obj" : $(SOURCE) $(DEP_CPP_CTANK) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CMLP.cpp
DEP_CPP_CMLP_=\
	".\StdAfx.h"\
	".\CMLP.h"\
	

"$(INTDIR)\CMLP.obj" : $(SOURCE) $(DEP_CPP_CMLP_) "$(INTDIR)"\
 "$(INTDIR)\Tanks.pch"


# End Source File
# End Target
# End Project
################################################################################
