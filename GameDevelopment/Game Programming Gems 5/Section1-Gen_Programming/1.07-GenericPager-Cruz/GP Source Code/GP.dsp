# Microsoft Developer Studio Project File - Name="GP" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=GP - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GP.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GP.mak" CFG="GP - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GP - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "GP - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GP - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GP_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GR /GX /O2 /I ".\incl" /D "C_GP" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GP_EXPORTS" /Yu"GPpch.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 /nologo /dll /machine:I386
# Begin Special Build Tool
OutDir=.\Release
TargetName=GP
SOURCE="$(InputPath)"
PostBuild_Cmds=echo ********* Post Build Command ***********	 copy $(OutDir)\$(TargetName).dll .\GPG5_GenericPagerTest\Release	echo DLLs copiadas	echo ********* End Post Build Command ***********
# End Special Build Tool

!ELSEIF  "$(CFG)" == "GP - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "GP_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GR /GX /ZI /Od /I ".\incl" /D "C_GP" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /Yu"GPpch.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /dll /debug /machine:I386
# Begin Special Build Tool
OutDir=.\Debug
TargetName=GP
SOURCE="$(InputPath)"
PostBuild_Cmds=echo ********* Post Build Command ***********	 copy $(OutDir)\$(TargetName).dll .\GPG5_GenericPagerTest\debug	echo DLLs copiadas	 copy $(OutDir)\$(TargetName).pdb .\GPG5_GenericPagerTest\debug	echo PDBs copiados	echo ********* End Post Build Command ***********
# End Special Build Tool

!ENDIF 

# Begin Target

# Name "GP - Win32 Release"
# Name "GP - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Threading Source Files"

# PROP Default_Filter "cpp"
# Begin Source File

SOURCE=.\src\GPlock.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPthread.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPthreadObject.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\GPindex.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPpch.cpp

!IF  "$(CFG)" == "GP - Win32 Release"

# ADD CPP /Yc

!ELSEIF  "$(CFG)" == "GP - Win32 Debug"

# ADD CPP /Yc"GPpch.h"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\GPsync.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPtile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPwindow.cpp
# End Source File
# Begin Source File

SOURCE=.\src\GPworld.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Threading Header Files"

# PROP Default_Filter "h"
# Begin Source File

SOURCE=.\incl\GPlock.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPthread.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPthreadObject.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\incl\GPdefs.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPindex.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPindex.types.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPpch.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPsync.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPtile.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPtile.types.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPwindow.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPwindow.types.h
# End Source File
# Begin Source File

SOURCE=.\incl\GPworld.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
