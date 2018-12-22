# Microsoft Developer Studio Generated NMAKE File, Format Version 4.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=GPExample - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to GPExample - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "GPExample - Win32 Release" && "$(CFG)" !=\
 "GPExample - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "GPExample.mak" CFG="GPExample - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GPExample - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GPExample - Win32 Debug" (based on "Win32 (x86) Application")
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
# PROP Target_Last_Scanned "GPExample - Win32 Debug"
RSC=rc.exe
MTL=mktyplib.exe
CPP=cl.exe

!IF  "$(CFG)" == "GPExample - Win32 Release"

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

ALL : "$(OUTDIR)\GPExample.exe"

CLEAN : 
	-@erase ".\Release\GPExample.exe"
	-@erase ".\Release\CGPTerminalNode.obj"
	-@erase ".\Release\GPExample.pch"
	-@erase ".\Release\CGPTernaryNode.obj"
	-@erase ".\Release\CGPTurnRightNode.obj"
	-@erase ".\Release\GPExampleView.obj"
	-@erase ".\Release\CGPUnaryNode.obj"
	-@erase ".\Release\CGPIsPoisonNode.obj"
	-@erase ".\Release\StdAfx.obj"
	-@erase ".\Release\CGPANDNode.obj"
	-@erase ".\Release\CGPNode.obj"
	-@erase ".\Release\CGPORNode.obj"
	-@erase ".\Release\CGPTurnLeftNode.obj"
	-@erase ".\Release\CGPNOTNode.obj"
	-@erase ".\Release\CGPBinaryNode.obj"
	-@erase ".\Release\GPExampleDoc.obj"
	-@erase ".\Release\CGPMoveForwardNode.obj"
	-@erase ".\Release\CGPRandomNumberNode.obj"
	-@erase ".\Release\MainFrm.obj"
	-@erase ".\Release\CGP.obj"
	-@erase ".\Release\GPExample.obj"
	-@erase ".\Release\CGPIfGreaterThanZeroNode.obj"
	-@erase ".\Release\CWorld.obj"
	-@erase ".\Release\CGPIsWallNode.obj"
	-@erase ".\Release\CGPIsFoodNode.obj"
	-@erase ".\Release\GPExample.res"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GPExample.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/"\
 /c 
CPP_OBJS=.\Release/
CPP_SBRS=
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "NDEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/GPExample.res" /d "NDEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GPExample.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/GPExample.pdb" /machine:I386 /out:"$(OUTDIR)/GPExample.exe" 
LINK32_OBJS= \
	"$(INTDIR)/CGPTerminalNode.obj" \
	"$(INTDIR)/CGPTernaryNode.obj" \
	"$(INTDIR)/CGPTurnRightNode.obj" \
	"$(INTDIR)/GPExampleView.obj" \
	"$(INTDIR)/CGPUnaryNode.obj" \
	"$(INTDIR)/CGPIsPoisonNode.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/CGPANDNode.obj" \
	"$(INTDIR)/CGPNode.obj" \
	"$(INTDIR)/CGPORNode.obj" \
	"$(INTDIR)/CGPTurnLeftNode.obj" \
	"$(INTDIR)/CGPNOTNode.obj" \
	"$(INTDIR)/CGPBinaryNode.obj" \
	"$(INTDIR)/GPExampleDoc.obj" \
	"$(INTDIR)/CGPMoveForwardNode.obj" \
	"$(INTDIR)/CGPRandomNumberNode.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/CGP.obj" \
	"$(INTDIR)/GPExample.obj" \
	"$(INTDIR)/CGPIfGreaterThanZeroNode.obj" \
	"$(INTDIR)/CWorld.obj" \
	"$(INTDIR)/CGPIsWallNode.obj" \
	"$(INTDIR)/CGPIsFoodNode.obj" \
	"$(INTDIR)/GPExample.res"

"$(OUTDIR)\GPExample.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "GPExample - Win32 Debug"

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

ALL : "$(OUTDIR)\GPExample.exe"

CLEAN : 
	-@erase ".\Debug\vc40.pdb"
	-@erase ".\Debug\GPExample.pch"
	-@erase ".\Debug\vc40.idb"
	-@erase ".\Debug\GPExample.exe"
	-@erase ".\Debug\CGP.obj"
	-@erase ".\Debug\CGPANDNode.obj"
	-@erase ".\Debug\CGPBinaryNode.obj"
	-@erase ".\Debug\StdAfx.obj"
	-@erase ".\Debug\CGPMoveForwardNode.obj"
	-@erase ".\Debug\CGPTurnRightNode.obj"
	-@erase ".\Debug\CGPIsPoisonNode.obj"
	-@erase ".\Debug\CGPNOTNode.obj"
	-@erase ".\Debug\CGPUnaryNode.obj"
	-@erase ".\Debug\CGPRandomNumberNode.obj"
	-@erase ".\Debug\CGPIsWallNode.obj"
	-@erase ".\Debug\CGPNode.obj"
	-@erase ".\Debug\CGPTerminalNode.obj"
	-@erase ".\Debug\CGPTurnLeftNode.obj"
	-@erase ".\Debug\MainFrm.obj"
	-@erase ".\Debug\GPExampleView.obj"
	-@erase ".\Debug\CWorld.obj"
	-@erase ".\Debug\CGPORNode.obj"
	-@erase ".\Debug\CGPTernaryNode.obj"
	-@erase ".\Debug\CGPIfGreaterThanZeroNode.obj"
	-@erase ".\Debug\GPExampleDoc.obj"
	-@erase ".\Debug\CGPIsFoodNode.obj"
	-@erase ".\Debug\GPExample.obj"
	-@erase ".\Debug\GPExample.res"
	-@erase ".\Debug\GPExample.ilk"
	-@erase ".\Debug\GPExample.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GPExample.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x809 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x809 /fo"$(INTDIR)/GPExample.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/GPExample.bsc" 
BSC32_SBRS=
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/GPExample.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/GPExample.exe" 
LINK32_OBJS= \
	"$(INTDIR)/CGP.obj" \
	"$(INTDIR)/CGPANDNode.obj" \
	"$(INTDIR)/CGPBinaryNode.obj" \
	"$(INTDIR)/StdAfx.obj" \
	"$(INTDIR)/CGPMoveForwardNode.obj" \
	"$(INTDIR)/CGPTurnRightNode.obj" \
	"$(INTDIR)/CGPIsPoisonNode.obj" \
	"$(INTDIR)/CGPNOTNode.obj" \
	"$(INTDIR)/CGPUnaryNode.obj" \
	"$(INTDIR)/CGPRandomNumberNode.obj" \
	"$(INTDIR)/CGPIsWallNode.obj" \
	"$(INTDIR)/CGPNode.obj" \
	"$(INTDIR)/CGPTerminalNode.obj" \
	"$(INTDIR)/CGPTurnLeftNode.obj" \
	"$(INTDIR)/MainFrm.obj" \
	"$(INTDIR)/GPExampleView.obj" \
	"$(INTDIR)/CWorld.obj" \
	"$(INTDIR)/CGPORNode.obj" \
	"$(INTDIR)/CGPTernaryNode.obj" \
	"$(INTDIR)/CGPIfGreaterThanZeroNode.obj" \
	"$(INTDIR)/GPExampleDoc.obj" \
	"$(INTDIR)/CGPIsFoodNode.obj" \
	"$(INTDIR)/GPExample.obj" \
	"$(INTDIR)/GPExample.res"

"$(OUTDIR)\GPExample.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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

# Name "GPExample - Win32 Release"
# Name "GPExample - Win32 Debug"

!IF  "$(CFG)" == "GPExample - Win32 Release"

!ELSEIF  "$(CFG)" == "GPExample - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "GPExample - Win32 Release"

!ELSEIF  "$(CFG)" == "GPExample - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\GPExample.cpp
DEP_CPP_GPEXA=\
	".\StdAfx.h"\
	".\GPExample.h"\
	".\MainFrm.h"\
	".\GPExampleDoc.h"\
	".\GPExampleView.h"\
	

"$(INTDIR)\GPExample.obj" : $(SOURCE) $(DEP_CPP_GPEXA) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "GPExample - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GPExample.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/"\
 /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GPExample.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "GPExample - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/GPExample.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\GPExample.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\StdAfx.h"\
	".\GPExample.h"\
	".\MainFrm.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GPExampleDoc.cpp
DEP_CPP_GPEXAM=\
	".\StdAfx.h"\
	".\GPExample.h"\
	".\GPExampleDoc.h"\
	".\CWorld.h"\
	".\CGP.h"\
	".\CGPNode.h"\
	".\CGPANDNode.h"\
	".\CGPIfGreaterThanZeroNode.h"\
	".\CGPIsFoodNode.h"\
	".\CGPIsPoisonNode.h"\
	".\CGPIsWallNode.h"\
	".\CGPMoveForwardNode.h"\
	".\CGPNOTNode.h"\
	".\CGPORNode.h"\
	".\CGPRandomNumberNode.h"\
	".\CGPTurnLeftNode.h"\
	".\CGPTurnRightNode.h"\
	".\CGPBinaryNode.h"\
	".\CGPTernaryNode.h"\
	".\CGPTerminalNode.h"\
	".\CGPUnaryNode.h"\
	

"$(INTDIR)\GPExampleDoc.obj" : $(SOURCE) $(DEP_CPP_GPEXAM) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GPExampleView.cpp
DEP_CPP_GPEXAMP=\
	".\StdAfx.h"\
	".\GPExample.h"\
	".\GPExampleDoc.h"\
	".\GPExampleView.h"\
	".\CWorld.h"\
	

"$(INTDIR)\GPExampleView.obj" : $(SOURCE) $(DEP_CPP_GPEXAMP) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\GPExample.rc
DEP_RSC_GPEXAMPL=\
	".\res\GPExample.ico"\
	".\res\GPExampleDoc.ico"\
	".\res\GPExample.rc2"\
	

"$(INTDIR)\GPExample.res" : $(SOURCE) $(DEP_RSC_GPEXAMPL) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPTurnRightNode.cpp
DEP_CPP_CGPTU=\
	".\StdAfx.h"\
	".\CGPTurnRightNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTerminalNode.h"\
	

"$(INTDIR)\CGPTurnRightNode.obj" : $(SOURCE) $(DEP_CPP_CGPTU) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPANDNode.cpp
DEP_CPP_CGPAN=\
	".\StdAfx.h"\
	".\CGPANDNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPBinaryNode.h"\
	

"$(INTDIR)\CGPANDNode.obj" : $(SOURCE) $(DEP_CPP_CGPAN) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPIfGreaterThanZeroNode.cpp
DEP_CPP_CGPIF=\
	".\StdAfx.h"\
	".\CGPIfGreaterThanZeroNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTernaryNode.h"\
	

"$(INTDIR)\CGPIfGreaterThanZeroNode.obj" : $(SOURCE) $(DEP_CPP_CGPIF)\
 "$(INTDIR)" "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPIsFoodNode.cpp
DEP_CPP_CGPIS=\
	".\StdAfx.h"\
	".\CGPIsFoodNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTerminalNode.h"\
	

"$(INTDIR)\CGPIsFoodNode.obj" : $(SOURCE) $(DEP_CPP_CGPIS) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPIsPoisonNode.cpp
DEP_CPP_CGPISP=\
	".\StdAfx.h"\
	".\CGPIsPoisonNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTerminalNode.h"\
	

"$(INTDIR)\CGPIsPoisonNode.obj" : $(SOURCE) $(DEP_CPP_CGPISP) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPIsWallNode.cpp
DEP_CPP_CGPISW=\
	".\StdAfx.h"\
	".\CGPIsWallNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTerminalNode.h"\
	

"$(INTDIR)\CGPIsWallNode.obj" : $(SOURCE) $(DEP_CPP_CGPISW) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPMoveForwardNode.cpp
DEP_CPP_CGPMO=\
	".\StdAfx.h"\
	".\CGPMoveForwardNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTerminalNode.h"\
	

"$(INTDIR)\CGPMoveForwardNode.obj" : $(SOURCE) $(DEP_CPP_CGPMO) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPNode.cpp
DEP_CPP_CGPNO=\
	".\StdAfx.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	

"$(INTDIR)\CGPNode.obj" : $(SOURCE) $(DEP_CPP_CGPNO) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPNOTNode.cpp
DEP_CPP_CGPNOT=\
	".\StdAfx.h"\
	".\CGPNOTNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPUnaryNode.h"\
	

"$(INTDIR)\CGPNOTNode.obj" : $(SOURCE) $(DEP_CPP_CGPNOT) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPORNode.cpp
DEP_CPP_CGPOR=\
	".\StdAfx.h"\
	".\CGPORNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPBinaryNode.h"\
	

"$(INTDIR)\CGPORNode.obj" : $(SOURCE) $(DEP_CPP_CGPOR) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPRandomNumberNode.cpp
DEP_CPP_CGPRA=\
	".\StdAfx.h"\
	".\CGPRandomNumberNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTerminalNode.h"\
	

"$(INTDIR)\CGPRandomNumberNode.obj" : $(SOURCE) $(DEP_CPP_CGPRA) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPTurnLeftNode.cpp
DEP_CPP_CGPTUR=\
	".\StdAfx.h"\
	".\CGPTurnLeftNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	".\CGPTerminalNode.h"\
	

"$(INTDIR)\CGPTurnLeftNode.obj" : $(SOURCE) $(DEP_CPP_CGPTUR) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGP.cpp
DEP_CPP_CGP_C=\
	".\StdAfx.h"\
	".\CGP.h"\
	".\CGPNode.h"\
	
NODEP_CPP_CGP_C=\
	".\CGPConstantNode.h"\
	

"$(INTDIR)\CGP.obj" : $(SOURCE) $(DEP_CPP_CGP_C) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CWorld.cpp
DEP_CPP_CWORL=\
	".\StdAfx.h"\
	".\CWorld.h"\
	

"$(INTDIR)\CWorld.obj" : $(SOURCE) $(DEP_CPP_CWORL) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPBinaryNode.cpp
DEP_CPP_CGPBI=\
	".\StdAfx.h"\
	".\CGPBinaryNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	

"$(INTDIR)\CGPBinaryNode.obj" : $(SOURCE) $(DEP_CPP_CGPBI) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPTerminalNode.cpp
DEP_CPP_CGPTE=\
	".\StdAfx.h"\
	".\CGPTerminalNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	

"$(INTDIR)\CGPTerminalNode.obj" : $(SOURCE) $(DEP_CPP_CGPTE) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPUnaryNode.cpp
DEP_CPP_CGPUN=\
	".\StdAfx.h"\
	".\CGPUnaryNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	

"$(INTDIR)\CGPUnaryNode.obj" : $(SOURCE) $(DEP_CPP_CGPUN) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\CGPTernaryNode.cpp
DEP_CPP_CGPTER=\
	".\StdAfx.h"\
	".\CGPTernaryNode.h"\
	".\CGPNode.h"\
	".\CGP.h"\
	

"$(INTDIR)\CGPTernaryNode.obj" : $(SOURCE) $(DEP_CPP_CGPTER) "$(INTDIR)"\
 "$(INTDIR)\GPExample.pch"


# End Source File
# End Target
# End Project
################################################################################
