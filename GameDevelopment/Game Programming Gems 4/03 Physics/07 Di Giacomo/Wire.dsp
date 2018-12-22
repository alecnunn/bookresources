# Microsoft Developer Studio Project File - Name="Wire" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Wire - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Wire.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Wire.mak" CFG="Wire - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Wire - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Wire - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Wire - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /map /machine:I386

!ELSEIF  "$(CFG)" == "Wire - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib opengl32.lib glut32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /profile /map /debug /machine:I386

!ENDIF 

# Begin Target

# Name "Wire - Win32 Release"
# Name "Wire - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "Physics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\Physics\CAxNod.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CEnvNod.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CMass.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CMyObj.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CNet.cpp
# End Source File
# End Group
# Begin Group "Gfx"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\Gfx\glutFunc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Gfx\trackball.cpp
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\Math\CAlpha.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Math\CHermInterp.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\main.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "Physics Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\Physics\CAxNod.h
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CEnvNod.h
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CMass.h
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CMyObj.h
# End Source File
# Begin Source File

SOURCE=.\src\Physics\CNet.h
# End Source File
# End Group
# Begin Group "Gfx Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\Gfx\glutFunc.h
# End Source File
# Begin Source File

SOURCE=.\src\Gfx\trackball.h
# End Source File
# End Group
# Begin Group "Math Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\Math\CAlpha.h
# End Source File
# Begin Source File

SOURCE=.\src\Math\CHermInterp.h
# End Source File
# Begin Source File

SOURCE=.\src\Math\tDef.h
# End Source File
# End Group
# End Group
# End Target
# End Project
