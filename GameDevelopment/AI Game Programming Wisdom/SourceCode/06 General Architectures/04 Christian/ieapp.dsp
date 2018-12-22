# Microsoft Developer Studio Project File - Name="ieapp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ieapp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ieapp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ieapp.mak" CFG="ieapp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ieapp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "ieapp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=snCl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ieapp - Win32 Release"

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
# ADD CPP /nologo /W3 /GR /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "ieapp - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=snLink.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ieapp - Win32 Release"
# Name "ieapp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\anim.cpp
# End Source File
# Begin Source File

SOURCE=.\cat.cpp
# End Source File
# Begin Source File

SOURCE=.\caught.cpp
# End Source File
# Begin Source File

SOURCE=.\character.cpp
# End Source File
# Begin Source File

SOURCE=.\chase.cpp
# End Source File
# Begin Source File

SOURCE=.\chased.cpp
# End Source File
# Begin Source File

SOURCE=.\damaged.cpp
# End Source File
# Begin Source File

SOURCE=.\damagereact.cpp
# End Source File
# Begin Source File

SOURCE=.\die.cpp
# End Source File
# Begin Source File

SOURCE=.\dog.cpp
# End Source File
# Begin Source File

SOURCE=.\done.cpp
# End Source File
# Begin Source File

SOURCE=.\dying.cpp
# End Source File
# Begin Source File

SOURCE=.\fight.cpp
# End Source File
# Begin Source File

SOURCE=.\flee.cpp
# End Source File
# Begin Source File

SOURCE=.\gametime.cpp
# End Source File
# Begin Source File

SOURCE=.\ie.cpp
# End Source File
# Begin Source File

SOURCE=.\ieapp.cpp
# End Source File
# Begin Source File

SOURCE=.\ieexec.cpp
# End Source File
# Begin Source File

SOURCE=.\iegoal.cpp
# End Source File
# Begin Source File

SOURCE=.\ieowner.cpp
# End Source File
# Begin Source File

SOURCE=.\ierule.cpp
# End Source File
# Begin Source File

SOURCE=.\nap.cpp
# End Source File
# Begin Source File

SOURCE=.\rested.cpp
# End Source File
# Begin Source File

SOURCE=.\seesEnemy.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# End Source File
# Begin Source File

SOURCE=.\tired.cpp
# End Source File
# Begin Source File

SOURCE=.\util.cpp
# End Source File
# Begin Source File

SOURCE=.\wander.cpp
# End Source File
# Begin Source File

SOURCE=.\wonfight.cpp
# End Source File
# Begin Source File

SOURCE=.\world2d.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\anim.h
# End Source File
# Begin Source File

SOURCE=.\cat.h
# End Source File
# Begin Source File

SOURCE=.\caught.h
# End Source File
# Begin Source File

SOURCE=.\character.h
# End Source File
# Begin Source File

SOURCE=.\characterbrainlib.h
# End Source File
# Begin Source File

SOURCE=.\chase.h
# End Source File
# Begin Source File

SOURCE=.\chased.h
# End Source File
# Begin Source File

SOURCE=.\damaged.h
# End Source File
# Begin Source File

SOURCE=.\die.h
# End Source File
# Begin Source File

SOURCE=.\dog.h
# End Source File
# Begin Source File

SOURCE=.\done.h
# End Source File
# Begin Source File

SOURCE=.\dying.h
# End Source File
# Begin Source File

SOURCE=.\fight.h
# End Source File
# Begin Source File

SOURCE=.\flee.h
# End Source File
# Begin Source File

SOURCE=.\gametime.h
# End Source File
# Begin Source File

SOURCE=.\ie.h
# End Source File
# Begin Source File

SOURCE=.\ieapp.h
# End Source File
# Begin Source File

SOURCE=.\ieexec.h
# End Source File
# Begin Source File

SOURCE=.\iegoal.h
# End Source File
# Begin Source File

SOURCE=.\ieowner.h
# End Source File
# Begin Source File

SOURCE=.\ierule.h
# End Source File
# Begin Source File

SOURCE=.\nap.h
# End Source File
# Begin Source File

SOURCE=.\npc.h
# End Source File
# Begin Source File

SOURCE=.\rested.h
# End Source File
# Begin Source File

SOURCE=.\seesEnemy.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\tired.h
# End Source File
# Begin Source File

SOURCE=.\util.h
# End Source File
# Begin Source File

SOURCE=.\wander.h
# End Source File
# Begin Source File

SOURCE=.\wonfight.h
# End Source File
# Begin Source File

SOURCE=.\world2d.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
