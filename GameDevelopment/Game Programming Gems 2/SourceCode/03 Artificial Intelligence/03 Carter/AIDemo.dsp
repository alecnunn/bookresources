# Microsoft Developer Studio Project File - Name="AIDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AIDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AIDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AIDemo.mak" CFG="AIDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AIDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AIDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AIDemo - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "\dev\stl" /I "include\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3dx8.lib d3d8.lib dinput8.lib winmm.lib kernel32.lib advapi32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# SUBTRACT LINK32 /incremental:yes

!ELSEIF  "$(CFG)" == "AIDemo - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /I "\dev\stl" /I "include\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 d3dx8.lib d3d8.lib dinput8.lib winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "AIDemo - Win32 Release"
# Name "AIDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\ai_app.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_app.hpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_base.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_base.hpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_fight.hpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_hungry.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_hungry.hpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_sleep.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_sleep.hpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_wander.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_behaviour_wander.hpp
# End Source File
# Begin Source File

SOURCE=.\ai_brain.cpp
# End Source File
# Begin Source File

SOURCE=.\ai_brain.hpp
# End Source File
# Begin Source File

SOURCE=.\base_class.cpp
# End Source File
# Begin Source File

SOURCE=.\base_class.hpp
# End Source File
# Begin Source File

SOURCE=.\entities.cpp
# End Source File
# Begin Source File

SOURCE=.\entities.hpp
# End Source File
# Begin Source File

SOURCE=.\core\fixedassert.cpp
# End Source File
# Begin Source File

SOURCE=.\core\handleexceptions.cpp
# End Source File
# Begin Source File

SOURCE=.\intelligent_pointer.hpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\microthreadmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\microthreads.cpp
# End Source File
# Begin Source File

SOURCE=.\microthreads.h
# End Source File
# Begin Source File

SOURCE=.\core\miscwinfunctions.cpp
# End Source File
# Begin Source File

SOURCE=.\core\precompiled.cpp
# End Source File
# Begin Source File

SOURCE=.\core\priv_coreprecompiled.h
# End Source File
# Begin Source File

SOURCE=.\core\randomnumbers.cpp
# End Source File
# Begin Source File

SOURCE=.\core\randomnumbers.h
# End Source File
# Begin Source File

SOURCE=.\standard_headers.hpp
# End Source File
# Begin Source File

SOURCE=.\utils.hpp
# End Source File
# End Group
# Begin Group "D3D"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\d3dapp.cpp
# End Source File
# Begin Source File

SOURCE=.\include\d3dapp.h
# End Source File
# Begin Source File

SOURCE=.\src\d3dfont.cpp
# End Source File
# Begin Source File

SOURCE=.\include\d3dfont.h
# End Source File
# Begin Source File

SOURCE=.\include\d3dres.h
# End Source File
# Begin Source File

SOURCE=.\src\d3dutil.cpp
# End Source File
# Begin Source File

SOURCE=.\include\d3dutil.h
# End Source File
# Begin Source File

SOURCE=.\src\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=.\include\dxutil.h
# End Source File
# End Group
# End Target
# End Project
