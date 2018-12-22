# Microsoft Developer Studio Project File - Name="NetLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=NetLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NetLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NetLib.mak" CFG="NetLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NetLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "NetLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NetLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "NetLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib\"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
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

# Name "NetLib - Win32 Release"
# Name "NetLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\cDataPacket.cpp
# End Source File
# Begin Source File

SOURCE=.\cHost.cpp
# End Source File
# Begin Source File

SOURCE=.\cMonitor.cpp
# End Source File
# Begin Source File

SOURCE=.\cNetClock.cpp
# End Source File
# Begin Source File

SOURCE=.\cNetError.cpp
# End Source File
# Begin Source File

SOURCE=.\cQueueIn.cpp
# End Source File
# Begin Source File

SOURCE=.\cQueueOut.cpp
# End Source File
# Begin Source File

SOURCE=.\cThread.cpp
# End Source File
# Begin Source File

SOURCE=.\cUnreliableQueueIn.cpp
# End Source File
# Begin Source File

SOURCE=.\cUnreliableQueueOut.cpp
# End Source File
# Begin Source File

SOURCE=.\MTUDP.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cDataCompress.h
# End Source File
# Begin Source File

SOURCE=.\cDataExtract.h
# End Source File
# Begin Source File

SOURCE=.\cDataPacket.h
# End Source File
# Begin Source File

SOURCE=.\cHost.h
# End Source File
# Begin Source File

SOURCE=.\cMonitor.h
# End Source File
# Begin Source File

SOURCE=.\cNetClock.h
# End Source File
# Begin Source File

SOURCE=.\cNetError.h
# End Source File
# Begin Source File

SOURCE=.\cQueueIn.h
# End Source File
# Begin Source File

SOURCE=.\cQueueOut.h
# End Source File
# Begin Source File

SOURCE=.\cThread.h
# End Source File
# Begin Source File

SOURCE=.\cUnreliableQueueIn.h
# End Source File
# Begin Source File

SOURCE=.\cUnreliableQueueOut.h
# End Source File
# Begin Source File

SOURCE=.\Macros.h
# End Source File
# Begin Source File

SOURCE=.\MTUDP.h
# End Source File
# End Group
# End Target
# End Project
