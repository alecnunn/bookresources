# Microsoft Developer Studio Project File - Name="ProceduralCloudDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ProceduralCloudDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ProceduralCloudDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ProceduralCloudDemo.mak" CFG="ProceduralCloudDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ProceduralCloudDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ProceduralCloudDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=xicl6.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ProceduralCloudDemo - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "ProceduralCloudDemo - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ProceduralCloudDemo - Win32 Release"
# Name "ProceduralCloudDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\Dx8Shell.rc
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawBackground.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DataIO\IawBmpLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCone.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCube.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCylinder.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IawD3dWrapper.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Utils\IawExponentialNoise.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DataIO\IawFileStream.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawGnomon.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DataIO\IawImageLoader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FrameWork\IawMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Utils\IawNoiseGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawObject.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FrameWork\IawRenderTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Shaders\IawShader.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Shaders\IawShaderMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawSkyPlane.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawTerrain.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawTerrainMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawTextMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FrameWork\IawTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\src\IawWindow3d.cpp
# End Source File
# Begin Source File

SOURCE=.\src\Main.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ProceduralCloudDemo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\RandTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StaticRandTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\src\stdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\Objects\IawBackground.h
# End Source File
# Begin Source File

SOURCE=.\src\DataIO\IawBmpLoader.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCamera.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCone.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCube.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawCylinder.h
# End Source File
# Begin Source File

SOURCE=.\src\IawD3dWrapper.h
# End Source File
# Begin Source File

SOURCE=.\src\DataIO\IawDataStream.h
# End Source File
# Begin Source File

SOURCE=.\src\Utils\IawExponentialNoise.h
# End Source File
# Begin Source File

SOURCE=.\src\DataIO\IawFileStream.h
# End Source File
# Begin Source File

SOURCE=.\src\IawFrameWork.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawGnomon.h
# End Source File
# Begin Source File

SOURCE=.\src\DataIO\IawImageLoader.h
# End Source File
# Begin Source File

SOURCE=.\src\FrameWork\IawMatrix.h
# End Source File
# Begin Source File

SOURCE=.\src\Utils\IawNoiseGenerator.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawObject.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawPlane.h
# End Source File
# Begin Source File

SOURCE=.\src\FrameWork\IawRenderTarget.h
# End Source File
# Begin Source File

SOURCE=.\src\Shaders\IawShader.h
# End Source File
# Begin Source File

SOURCE=.\src\Shaders\IawShaderMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawSkyPlane.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawSphere.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawSuperVertex.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawTerrain.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawTerrainMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\Objects\IawTextMgr.h
# End Source File
# Begin Source File

SOURCE=.\src\FrameWork\IawTexture.h
# End Source File
# Begin Source File

SOURCE=.\src\IawWindow3d.h
# End Source File
# Begin Source File

SOURCE=.\src\Utils\MissingDX7Stuff.h
# End Source File
# Begin Source File

SOURCE=.\src\ProceduralCloudDemo.h
# End Source File
# Begin Source File

SOURCE=.\src\RandTexture.h
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\StaticRandTexture.h
# End Source File
# Begin Source File

SOURCE=.\src\stdAfx.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
