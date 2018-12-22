# Microsoft Developer Studio Project File - Name="GameLib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=GameLib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GameLib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GameLib.mak" CFG="GameLib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GameLib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "GameLib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GameLib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "GameLib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W2 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"stdafx.h" /FD /GZ /c
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

# Name "GameLib - Win32 Release"
# Name "GameLib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Application.cpp
# End Source File
# Begin Source File

SOURCE=.\File.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Application.h
# End Source File
# Begin Source File

SOURCE=.\File.h
# End Source File
# Begin Source File

SOURCE=.\GameErrors.h
# End Source File
# Begin Source File

SOURCE=.\GameGlobals.h
# End Source File
# Begin Source File

SOURCE=.\GameTypes.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Windows code"

# PROP Default_Filter "*.cpp, *.h"
# Begin Source File

SOURCE=.\Window.cpp
# End Source File
# Begin Source File

SOURCE=.\Window.h
# End Source File
# End Group
# Begin Group "DX Headers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=D:\mssdk7\include\d3dcaps.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3drm.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3drmdef.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3drmobj.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3drmwin.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dtypes.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dvec.inl
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dx.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dxcore.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dxerr.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dxmath.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dxmath.inl
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dxshapes.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\d3dxsprite.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dinput.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dinputd.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dls1.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dls2.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dmdls.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dmerror.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dmksctrl.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dmusbuff.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dmusicc.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dmusicf.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dmusici.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dplay.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dplobby.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dsetup.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dsound.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dvp.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\dxfile.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\multimon.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\rmxfguid.h
# End Source File
# Begin Source File

SOURCE=D:\mssdk7\include\rmxftmpl.h
# End Source File
# End Group
# Begin Group "Graphics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DxHelper.h
# End Source File
# Begin Source File

SOURCE=.\GraphicsLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\GraphicsLayer.h
# End Source File
# Begin Source File

SOURCE=.\Model.cpp
# End Source File
# Begin Source File

SOURCE=.\Model.h
# End Source File
# End Group
# Begin Group "Sound"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Sound.cpp

!IF  "$(CFG)" == "GameLib - Win32 Release"

!ELSEIF  "$(CFG)" == "GameLib - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\SoundLayer.cpp

!IF  "$(CFG)" == "GameLib - Win32 Release"

!ELSEIF  "$(CFG)" == "GameLib - Win32 Debug"

# ADD CPP /Yu

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\SoundLayer.h
# End Source File
# Begin Source File

SOURCE=.\wavread.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\wavread.h
# End Source File
# End Group
# Begin Group "Input"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\InputLayer.cpp
# End Source File
# Begin Source File

SOURCE=.\InputLayer.h
# End Source File
# Begin Source File

SOURCE=.\Keyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\Keyboard.h
# End Source File
# Begin Source File

SOURCE=.\Mouse.cpp
# End Source File
# Begin Source File

SOURCE=.\Mouse.h
# End Source File
# End Group
# End Target
# End Project
