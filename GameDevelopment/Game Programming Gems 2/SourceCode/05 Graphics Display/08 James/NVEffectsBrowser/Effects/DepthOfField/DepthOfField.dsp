# Microsoft Developer Studio Project File - Name="DepthOfField" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=DepthOfField - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "DepthOfField.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "DepthOfField.mak" CFG="DepthOfField - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "DepthOfField - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "DepthOfField - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "mwloka-win2k"
# PROP Scc_LocalPath "..\..\.."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEPTHOFFIELD_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\..\..\include" /I "..\..\shared" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEPTHOFFIELD_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8.lib /nologo /dll /machine:I386 /out:"DepthOfField.dll"

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEPTHOFFIELD_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /I "..\..\shared" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "DEPTHOFFIELD_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8.lib /nologo /dll /debug /machine:I386 /out:"DepthOfField.dll" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "DepthOfField - Win32 Release"
# Name "DepthOfField - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Blur.nvp

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\Blur.nvp

"Blur.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Blur.nvp

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\Blur.nvp

"Blur.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Blur.nvp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Blur.nvv

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\Blur.nvv

"Blur.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Blur.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\Blur.nvv

"Blur.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Blur.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\DepthOfField.cpp
# End Source File
# Begin Source File

SOURCE=.\DepthOfFieldBlend.nvp

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\DepthOfFieldBlend.nvp

"DepthOfFieldBlend.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe DepthOfFieldBlend.nvp

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\DepthOfFieldBlend.nvp

"DepthOfFieldBlend.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe DepthOfFieldBlend.nvp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\shared\eb_property.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonSrc\NV_Error.cpp
# End Source File
# Begin Source File

SOURCE=..\..\..\CommonSrc\nvtex.cpp
# End Source File
# Begin Source File

SOURCE=.\ShowBlurriness.nvp

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\ShowBlurriness.nvp

"ShowBlurriness.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe ShowBlurriness.nvp

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\ShowBlurriness.nvp

"ShowBlurriness.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe ShowBlurriness.nvp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Tetrahedron.nvp

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\Tetrahedron.nvp

"Tetrahedron.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Tetrahedron.nvp

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\Tetrahedron.nvp

"Tetrahedron.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Tetrahedron.nvp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Tetrahedron.nvv

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\Tetrahedron.nvv

"Tetrahedron.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Tetrahedron.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\Tetrahedron.nvv

"Tetrahedron.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe Tetrahedron.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\TetrahedronNoDoFLookup.nvp

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\TetrahedronNoDoFLookup.nvp

"TetrahedronNoDoFLookup.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe TetrahedronNoDoFLookup.nvp

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\TetrahedronNoDoFLookup.nvp

"TetrahedronNoDoFLookup.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe TetrahedronNoDoFLookup.nvp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\World.nvp

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\World.nvp

"World.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe World.nvp

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\World.nvp

"World.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe World.nvp

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\World.nvv

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\World.nvv

"World.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe World.nvv

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\World.nvv

"World.vso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe World.nvv

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\WorldNoDoFLookup.nvp

!IF  "$(CFG)" == "DepthOfField - Win32 Release"

# Begin Custom Build
InputPath=.\WorldNoDoFLookup.nvp

"WorldNoDoFLookup.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe WorldNoDoFLookup.nvp

# End Custom Build

!ELSEIF  "$(CFG)" == "DepthOfField - Win32 Debug"

# Begin Custom Build
InputPath=.\WorldNoDoFLookup.nvp

"WorldNoDoFLookup.pso" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	..\..\nvasm.exe WorldNoDoFLookup.nvp

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Constants.h
# End Source File
# Begin Source File

SOURCE=.\DepthOfField.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ObjectTexture.dds
# End Source File
# Begin Source File

SOURCE=.\skybox_back.dds
# End Source File
# Begin Source File

SOURCE=.\skybox_front.dds
# End Source File
# Begin Source File

SOURCE=.\skybox_left.dds
# End Source File
# Begin Source File

SOURCE=.\skybox_right.dds
# End Source File
# Begin Source File

SOURCE=.\skybox_top.dds
# End Source File
# Begin Source File

SOURCE=.\wood.dds
# End Source File
# End Group
# End Target
# End Project
