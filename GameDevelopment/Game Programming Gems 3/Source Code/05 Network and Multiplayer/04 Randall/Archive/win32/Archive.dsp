# Microsoft Developer Studio Project File - Name="Archive" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Archive - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Archive.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Archive.mak" CFG="Archive - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Archive - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Archive - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Archive"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Archive - Win32 Release"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /WX /GR /GX /O2 /I "..\..\\" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"ArchivePCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Archive - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Archive___Win32_Debug"
# PROP BASE Intermediate_Dir "Archive___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Archive___Win32_Debug"
# PROP Intermediate_Dir "Archive___Win32_Debug"
# PROP Target_Dir ""
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /WX /Gm /GR /GX /ZI /Od /I "..\..\\" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"ArchivePCH.h" /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "Archive - Win32 Release"
# Name "Archive - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\ArchivePCH.cpp
# ADD CPP /Yc"ArchivePCH.h"
# End Source File
# Begin Source File

SOURCE=..\AutoArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\DeltaArchive.cpp
# End Source File
# Begin Source File

SOURCE=..\Stream.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Archive.h
# End Source File
# Begin Source File

SOURCE=..\ArchivePCH.h
# End Source File
# Begin Source File

SOURCE=..\AutoArchive.h
# End Source File
# Begin Source File

SOURCE=..\AutoVariable.h
# End Source File
# Begin Source File

SOURCE=..\AutoVariableWatcher.h
# End Source File
# Begin Source File

SOURCE=..\DeltaArchive.h
# End Source File
# Begin Source File

SOURCE=..\DeltaContainer.h
# End Source File
# Begin Source File

SOURCE=..\DeltaMap.h
# End Source File
# Begin Source File

SOURCE=..\DeltaVariable.h
# End Source File
# Begin Source File

SOURCE=..\DeltaVector.h
# End Source File
# Begin Source File

SOURCE=..\Stream.h
# End Source File
# Begin Source File

SOURCE=..\TypeMap.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\win32\Project.lnt
# End Source File
# End Target
# End Project
