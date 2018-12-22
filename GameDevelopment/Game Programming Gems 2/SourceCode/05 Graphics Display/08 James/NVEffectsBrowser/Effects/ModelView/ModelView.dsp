# Microsoft Developer Studio Project File - Name="ModelView" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=ModelView - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ModelView.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ModelView.mak" CFG="ModelView - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ModelView - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "ModelView - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "ModelView"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ModelView - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "ModelView___Win32_Release"
# PROP BASE Intermediate_Dir "ModelView___Win32_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODELVIEW_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "..\..\shared" /I "..\..\..\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODELVIEW_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8.lib /nologo /dll /machine:I386 /out:"ModelView.dll"

!ELSEIF  "$(CFG)" == "ModelView - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ModelView___Win32_Debug"
# PROP BASE Intermediate_Dir "ModelView___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODELVIEW_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\shared" /I "..\..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "MODELVIEW_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8.lib /nologo /dll /debug /machine:I386 /out:"ModelView.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ModelView - Win32 Release"
# Name "ModelView - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DirectionalLight.nvv

!IF  "$(CFG)" == "ModelView - Win32 Release"

USERDEP__DIREC="DirectionalLight.h"	
# Begin Custom Build
InputPath=.\DirectionalLight.nvv

"DirectionalLight.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm DirectionalLight.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "ModelView - Win32 Debug"

USERDEP__DIREC="DirectionalLight.h"	
# Begin Custom Build
InputPath=.\DirectionalLight.nvv

"DirectionalLight.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm DirectionalLight.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DirectionalLightBump.nvv

!IF  "$(CFG)" == "ModelView - Win32 Release"

# Begin Custom Build
InputPath=.\DirectionalLightBump.nvv

"DirectionalLightBump.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm DirectionalLightBump.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "ModelView - Win32 Debug"

# Begin Custom Build
InputPath=.\DirectionalLightBump.nvv

"DirectionalLightBump.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm DirectionalLightBump.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\eb_property.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonSrc\nvcomplexmesh.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonSrc\nvcomplexmeshpass.cpp
# End Source File
# Begin Source File

SOURCE=.\shader_ModelView.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DirectionalLight.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvcomplexmesh.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvcomplexmeshpass.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvinc.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvmaterialresource.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvresourcemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvstatemanager.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvtexture.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvtextureresource.h
# End Source File
# Begin Source File

SOURCE=.\shader_ModelView.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\singleton.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
