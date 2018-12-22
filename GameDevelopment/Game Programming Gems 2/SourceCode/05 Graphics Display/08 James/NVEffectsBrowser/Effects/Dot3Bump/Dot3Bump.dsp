# Microsoft Developer Studio Project File - Name="Dot3Bump" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=Dot3Bump - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Dot3Bump.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Dot3Bump.mak" CFG="Dot3Bump - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Dot3Bump - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "Dot3Bump - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Dot3Bump"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Dot3Bump - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOT3BUMP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOT3BUMP_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 d3dx8.lib winmm.lib gdi32.lib user32.lib advapi32.lib comdlg32.lib /nologo /dll /machine:I386 /out:"dot3bump.dll"

!ELSEIF  "$(CFG)" == "Dot3Bump - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOT3BUMP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DOT3BUMP_EXPORTS" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx8dt.lib winmm.lib gdi32.lib user32.lib advapi32.lib comdlg32.lib /nologo /dll /debug /machine:I386 /out:"dot3bump.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Dot3Bump - Win32 Release"
# Name "Dot3Bump - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\dot3_directional.nvv

!IF  "$(CFG)" == "Dot3Bump - Win32 Release"

# Begin Custom Build
InputPath=.\dot3_directional.nvv

"dot3_directional.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_directional.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "Dot3Bump - Win32 Debug"

# Begin Custom Build
InputPath=.\dot3_directional.nvv

"dot3_directional.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_directional.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dot3_lightobject.nvv

!IF  "$(CFG)" == "Dot3Bump - Win32 Release"

# Begin Custom Build
InputPath=.\dot3_lightobject.nvv

"dot3_lightobject.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_lightobject.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "Dot3Bump - Win32 Debug"

# Begin Custom Build
InputPath=.\dot3_lightobject.nvv

"dot3_lightobject.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_lightobject.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dot3_point.nvv

!IF  "$(CFG)" == "Dot3Bump - Win32 Release"

# Begin Custom Build
InputPath=.\dot3_point.nvv

"dot3_point.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_point.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "Dot3Bump - Win32 Debug"

# Begin Custom Build
InputPath=.\dot3_point.nvv

"dot3_point.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_point.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dot3_point_atten.nvv

!IF  "$(CFG)" == "Dot3Bump - Win32 Release"

# Begin Custom Build
InputPath=.\dot3_point_atten.nvv

"dot3_point_atten.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_point_atten.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "Dot3Bump - Win32 Debug"

# Begin Custom Build
InputPath=.\dot3_point_atten.nvv

"dot3_point_atten.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_point_atten.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\dot3_transform.nvv

!IF  "$(CFG)" == "Dot3Bump - Win32 Release"

# Begin Custom Build
InputPath=.\dot3_transform.nvv

"dot3_transform.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_transform.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "Dot3Bump - Win32 Debug"

# Begin Custom Build
InputPath=.\dot3_transform.nvv

"dot3_transform.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm -d dot3_transform.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\eb_property.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonSrc\nvmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\shader_Dot3_Directional.cpp
# End Source File
# Begin Source File

SOURCE=.\shader_Dot3_Point.cpp
# End Source File
# Begin Source File

SOURCE=.\shader_Dot3_Util.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\dot3.h
# End Source File
# Begin Source File

SOURCE=..\..\..\Include\nvtexture.h
# End Source File
# Begin Source File

SOURCE=.\shader_Dot3_Directional.h
# End Source File
# Begin Source File

SOURCE=.\shader_Dot3_Point.h
# End Source File
# Begin Source File

SOURCE=.\shader_Dot3_Util.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
