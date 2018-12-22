# Microsoft Developer Studio Project File - Name="Driver of Death" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Driver of Death - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Driver of Death.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Driver of Death.mak" CFG="Driver of Death - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Driver of Death - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Driver of Death - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Driver of Death - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Driver of Death - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Driver of Death - Win32 Release"
# Name "Driver of Death - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=".\Driver of Death.cpp"
# End Source File
# Begin Source File

SOURCE=".\Driver of Death.rc"
# End Source File
# Begin Source File

SOURCE=".\Driver of DeathDlg.cpp"
# End Source File
# Begin Source File

SOURCE=.\FingersOfDeath.cpp
# End Source File
# Begin Source File

SOURCE=.\MyTabCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TabEightDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabFiveDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabFourDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabNineDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabOneDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSevenDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabSixDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabTenDLG.cpp
# End Source File
# Begin Source File

SOURCE=.\TabThreeDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TabTwoDlg.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=".\Driver of Death.h"
# End Source File
# Begin Source File

SOURCE=".\Driver of DeathDlg.h"
# End Source File
# Begin Source File

SOURCE=.\FingersOfDeath.h
# End Source File
# Begin Source File

SOURCE=.\MyTabCtrl.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TabEightDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabFiveDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabFourDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabNineDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabOneDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabSevenDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabSixDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabTenDLG.h
# End Source File
# Begin Source File

SOURCE=.\TabThreeDlg.h
# End Source File
# Begin Source File

SOURCE=.\TabTwoDlg.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=".\res\Area Magic 2.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Beating the Bushes With Prey Spacing.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Beating the Bushes.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Chemicals, Fireballs and Area Magic.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Death By Walking Artillery.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Driver of Death.ico"
# End Source File
# Begin Source File

SOURCE=".\res\Driver of Death.rc2"
# End Source File
# Begin Source File

SOURCE=".\res\Hitting A Rectangle.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Hitting A Ribbon.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Hitting The Bullseye.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Kills Come in Four Flavors.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Shotgunning A Small Target.bmp"
# End Source File
# Begin Source File

SOURCE=".\res\Shrapnel Wedge.bmp"
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
