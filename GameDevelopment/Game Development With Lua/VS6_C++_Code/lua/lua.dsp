# Microsoft Developer Studio Project File - Name="lua" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=lua - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "lua.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "lua.mak" CFG="lua - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "lua - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "lua - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "lua"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "lua - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "lua-5.0.2\include" /I "..\include" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "lua - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\include" /I "lua-5.0.2\include" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
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

# Name "lua - Win32 Release"
# Name "lua - Win32 Debug"
# Begin Group "Lua 5.0.2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\lua-5.0.2\src\lapi.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lapi.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\lauxlib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\lbaselib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lcode.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lcode.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\ldblib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ldebug.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ldebug.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ldo.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ldo.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ldump.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lfunc.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lfunc.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lgc.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lgc.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\liolib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\llex.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\llex.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\llimits.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\lmathlib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lmem.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lmem.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\loadlib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lobject.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lobject.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lopcodes.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lopcodes.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lparser.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lparser.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lstate.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lstate.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lstring.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lstring.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\lstrlib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ltable.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ltable.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lib\ltablib.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ltests.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ltm.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\ltm.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lundump.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lundump.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lvm.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lvm.h"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lzio.c"
# End Source File
# Begin Source File

SOURCE=".\lua-5.0.2\src\lzio.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\cLua.cpp
# End Source File
# Begin Source File

SOURCE=..\include\cLua.h
# End Source File
# End Target
# End Project
