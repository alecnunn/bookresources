# Microsoft Developer Studio Project File - Name="NormalMapGen" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=NormalMapGen - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "NormalMapGen.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "NormalMapGen.mak" CFG="NormalMapGen - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "NormalMapGen - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "NormalMapGen - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "NormalMapGen - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NORMALMAPGEN_EXPORTS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NORMALMAPGEN_EXPORTS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40a /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386 /out:"NormalMapGen.dlo"

!ELSEIF  "$(CFG)" == "NormalMapGen - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NORMALMAPGEN_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "NORMALMAPGEN_EXPORTS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40a /d "_DEBUG"
# ADD RSC /l 0x40a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /out:"NormalMapGen.dlo" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "NormalMapGen - Win32 Release"
# Name "NormalMapGen - Win32 Debug"
# Begin Group "Resources"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\NormalMapGen.rc
# End Source File
# Begin Source File

SOURCE=.\NormalMapGenr.def
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Array.h
# End Source File
# Begin Source File

SOURCE=.\Maths.h
# End Source File
# Begin Source File

SOURCE=.\NormalMapGen.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MapGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\MaxInterface.cpp
# End Source File
# Begin Source File

SOURCE=.\Plugin.cpp
# End Source File
# Begin Source File

SOURCE=.\Point.cpp
# End Source File
# End Group
# Begin Group "External Libs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\acap.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\viewfile.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\tessint.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\SpringSys.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\RenderUtil.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\Poly.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\particle.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\Paramblk2.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\MNMath.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\mesh.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\menus.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\MenuMan.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\maxutil.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\Maxscrpt.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\maxnet.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\ManipSys.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\imageViewers.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\helpsys.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\gup.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\gfx.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\geom.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\gcomm2.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\gcomm.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\flt.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\FLILIBR.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\FLILIBH.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\FLILIBD.LIB
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\expr.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\edmodel.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\CustDlg.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\core.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\client.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\bmm.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\..\3dsmax4\maxsdk\lib\zlibdll.lib
# End Source File
# End Group
# End Target
# End Project
