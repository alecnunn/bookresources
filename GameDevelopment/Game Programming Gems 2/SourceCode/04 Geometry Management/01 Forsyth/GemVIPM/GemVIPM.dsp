# Microsoft Developer Studio Project File - Name="GemVIPM" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GemVIPM - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GemVIPM.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GemVIPM.mak" CFG="GemVIPM - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GemVIPM - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GemVIPM - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GemVIPM - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /Zi /O2 /I "..\d3d8frame\include" /I "..\MyInc" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D DXVERSION=8 /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "NDEBUG"
# ADD RSC /l 0x809 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8dt.lib d3d8.lib ddraw.lib d3dxof.lib dxguid.lib winmm.lib dinput8.lib /nologo /subsystem:windows /debug /machine:I386

!ELSEIF  "$(CFG)" == "GemVIPM - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "GemVIPM___Win32_Debug"
# PROP BASE Intermediate_Dir "GemVIPM___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\d3d8frame\include" /I "..\MyInc" /D "_DEBUG" /D "DEBUG" /D "WIN32" /D "_WINDOWS" /D DXVERSION=8 /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x809 /d "_DEBUG"
# ADD RSC /l 0x809 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib d3dx8dt.lib d3d8.lib ddraw.lib d3dxof.lib dxguid.lib winmm.lib dinput8.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GemVIPM - Win32 Release"
# Name "GemVIPM - Win32 Debug"
# Begin Group "MyInc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\MyInc\mesh.h
# End Source File
# Begin Source File

SOURCE=..\MyInc\TomsD3DLib.cpp
# End Source File
# Begin Source File

SOURCE=..\MyInc\TomsD3DLib.h
# End Source File
# Begin Source File

SOURCE=..\MyInc\TomsLib.cpp
# End Source File
# Begin Source File

SOURCE=..\MyInc\TomsLib.h
# End Source File
# End Group
# Begin Group "D3D files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\mssdk\include\basetsd.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3d8.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3d8caps.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3d8types.h
# End Source File
# Begin Source File

SOURCE=..\D3D8FRAME\include\d3dapp.h
# End Source File
# Begin Source File

SOURCE=..\D3D8FRAME\include\d3dfont.h
# End Source File
# Begin Source File

SOURCE=..\D3D8FRAME\include\d3dutil.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8core.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8effect.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8math.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8math.inl
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8mesh.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8shape.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\d3dx8tex.h
# End Source File
# Begin Source File

SOURCE=..\..\mssdk\include\dxfile.h
# End Source File
# Begin Source File

SOURCE=..\D3D8FRAME\include\dxutil.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\DirectX.ico
# End Source File
# Begin Source File

SOURCE=.\gemvipm.cpp
# End Source File
# Begin Source File

SOURCE=.\object.cpp
# End Source File
# Begin Source File

SOURCE=.\object.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\WinMain.rc
# End Source File
# End Target
# End Project
