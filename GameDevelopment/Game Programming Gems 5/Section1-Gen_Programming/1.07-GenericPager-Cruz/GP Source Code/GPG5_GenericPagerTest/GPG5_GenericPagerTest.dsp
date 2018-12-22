# Microsoft Developer Studio Project File - Name="GPG5_gpTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GPG5_gpTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GPG5_GenericPagerTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GPG5_GenericPagerTest.mak" CFG="GPG5_gpTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GPG5_gpTest - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GPG5_gpTest - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GPG5_gpTest - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I ".\incl" /I "..\incl" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"GPG5_gpPch.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 user32.lib gdi32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "GPG5_gpTest - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I ".\incl" /I "..\incl" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"GPG5_gpPch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 user32.lib gdi32.lib /nologo /subsystem:windows /debug /machine:I386

!ENDIF 

# Begin Target

# Name "GPG5_gpTest - Win32 Release"
# Name "GPG5_gpTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "GP Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\src\GPG5_gpNodeTile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNodeWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNodeWorld.cpp
# End Source File
# End Group
# Begin Group "Simple Engine Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\src\GPG5_gpBaseNodeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpCamera2D.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpEventManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNode.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNodeAxis.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNodeGrid.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNodeManager.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNodeQuad.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpNodeTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpVector.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpWindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpWindowOpenGL.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\GPG5_gpMain.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPG5_gpPch.cpp
# ADD CPP /Yc"GPG5_gpPch.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "GP Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeTile.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeWindow.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeWorld.h
# End Source File
# End Group
# Begin Group "Simple Engine Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\incl\GPG5_gpBaseNodeManager.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpCamera2D.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpEventManager.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNode.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNode.types.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeAxis.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeGrid.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeManager.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeQuad.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpNodeTexture.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpVector.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpWindow.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPG5_gpWindowOpenGL.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\incl\GPG5_gpPch.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Lib"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=..\Debug\GP.lib

!IF  "$(CFG)" == "GPG5_gpTest - Win32 Release"

# PROP Exclude_From_Build 1

!ELSEIF  "$(CFG)" == "GPG5_gpTest - Win32 Debug"

!ENDIF 

# End Source File
# End Group
# Begin Group "Lib Release"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE=..\Release\GP.lib

!IF  "$(CFG)" == "GPG5_gpTest - Win32 Release"

!ELSEIF  "$(CFG)" == "GPG5_gpTest - Win32 Debug"

# PROP Exclude_From_Build 1

!ENDIF 

# End Source File
# End Group
# Begin Group "Common Lib"

# PROP Default_Filter "lib"
# Begin Source File

SOURCE="C:\Archivos de programa\Microsoft Visual Studio\VC98\Lib\OPENGL32.LIB"
# End Source File
# Begin Source File

SOURCE="C:\Archivos de programa\Microsoft Visual Studio\VC98\Lib\GLU32.LIB"
# End Source File
# End Group
# End Target
# End Project
