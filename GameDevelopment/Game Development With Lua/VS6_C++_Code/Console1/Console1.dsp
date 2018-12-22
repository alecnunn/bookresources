# Microsoft Developer Studio Project File - Name="Console1" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Console1 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Console1.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Console1.mak" CFG="Console1 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Console1 - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Console1 - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Console1"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Console1 - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\include" /I "..\lua\lua-5.0.2\include" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "Console1 - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\include" /I "..\lua\lua-5.0.2\include" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Console1 - Win32 Release"
# Name "Console1 - Win32 Debug"
# Begin Group "Lua"

# PROP Default_Filter ""
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lapi.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lapi.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\lauxlib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\lbaselib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lcode.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lcode.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\ldblib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ldebug.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ldebug.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ldo.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ldo.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ldump.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lfunc.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lfunc.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lgc.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lgc.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\liolib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\llex.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\llex.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\llimits.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\lmathlib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lmem.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lmem.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\loadlib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lobject.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lobject.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lopcodes.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lopcodes.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lparser.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lparser.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lstate.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lstate.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lstring.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lstring.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\lstrlib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ltable.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ltable.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lib\ltablib.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ltests.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ltm.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\ltm.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lundump.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lundump.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lvm.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lvm.h"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lzio.c"
# End Source File
# Begin Source File

SOURCE="..\lua\lua-5.0.2\src\lzio.h"
# End Source File
# End Group
# Begin Source File

SOURCE=.\Console1.cpp
# End Source File
# End Target
# End Project
