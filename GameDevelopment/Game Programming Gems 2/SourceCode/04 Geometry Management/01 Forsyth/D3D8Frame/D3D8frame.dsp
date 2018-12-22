# Microsoft Developer Studio Project File - Name="D3D8frame" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=D3D8frame - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "D3D8frame.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "D3D8frame.mak" CFG="D3D8frame - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "D3D8frame - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "D3D8frame - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "D3D8frame - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "include" /I "..\MyInc" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D DXVERSION=8 /YX /FD /c
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "D3D8frame - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "D3D8frame___Win32_Debug"
# PROP BASE Intermediate_Dir "D3D8frame___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "include" /I "..\MyInc" /D "DEBUG" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D DXVERSION=8 /YX /FD /GZ /c
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "D3D8frame - Win32 Release"
# Name "D3D8frame - Win32 Debug"
# Begin Group "Include"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\include\d3dapp.h
# End Source File
# Begin Source File

SOURCE=.\include\d3dfile.h
# End Source File
# Begin Source File

SOURCE=.\include\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\include\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\include\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\include\ddutil.h
# End Source File
# Begin Source File

SOURCE=.\include\didcfgview.h
# End Source File
# Begin Source File

SOURCE=.\include\diutil.h
# End Source File
# Begin Source File

SOURCE=.\include\dmutil.h
# End Source File
# Begin Source File

SOURCE=.\include\dsutil.h
# End Source File
# Begin Source File

SOURCE=.\include\dxutil.h
# End Source File
# Begin Source File

SOURCE=.\include\netclient.h
# End Source File
# Begin Source File

SOURCE=.\include\netclientres.h
# End Source File
# Begin Source File

SOURCE=.\include\netconnect.h
# End Source File
# Begin Source File

SOURCE=.\include\netconnectres.h
# End Source File
# Begin Source File

SOURCE=.\include\netvoice.h
# End Source File
# End Group
# Begin Group "Src"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\src\d3dfile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\src\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\didcfgview.cpp
# End Source File
# Begin Source File

SOURCE=.\src\diutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dmutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dsutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\src\netclient.cpp
# End Source File
# Begin Source File

SOURCE=.\src\netconnect.cpp
# End Source File
# Begin Source File

SOURCE=.\src\netvoice.cpp
# End Source File
# End Group
# End Target
# End Project
