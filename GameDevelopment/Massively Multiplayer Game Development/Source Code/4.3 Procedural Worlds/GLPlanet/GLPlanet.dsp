# Microsoft Developer Studio Project File - Name="GLPlanet" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GLPlanet - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GLPlanet.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GLPlanet.mak" CFG="GLPlanet - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GLPlanet - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GLPlanet - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GLPlanet - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /G5 /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"Master.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib ole32.lib winmm.lib opengl32.lib glu32.lib dinput.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "GLPlanet - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /G5 /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"Master.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib advapi32.lib ole32.lib winmm.lib opengl32.lib glu32.lib dinput.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GLPlanet - Win32 Release"
# Name "GLPlanet - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Billboard.cpp
# End Source File
# Begin Source File

SOURCE=.\DXUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\GameApp.cpp
# End Source File
# Begin Source File

SOURCE=.\GameEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\GLPlanet.rc
# End Source File
# Begin Source File

SOURCE=.\GLUtil.cpp
# End Source File
# Begin Source File

SOURCE=.\Impostor.cpp
# End Source File
# Begin Source File

SOURCE=.\Master.cpp
# ADD CPP /Yc"Master.h"
# End Source File
# Begin Source File

SOURCE=.\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Noise.cpp
# End Source File
# Begin Source File

SOURCE=.\PBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\PixelBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Texture.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Billboard.h
# End Source File
# Begin Source File

SOURCE=.\DXUtil.h
# End Source File
# Begin Source File

SOURCE=.\Font.h
# End Source File
# Begin Source File

SOURCE=.\GameApp.h
# End Source File
# Begin Source File

SOURCE=.\GameEngine.h
# End Source File
# Begin Source File

SOURCE=.\GLUtil.h
# End Source File
# Begin Source File

SOURCE=.\Impostor.h
# End Source File
# Begin Source File

SOURCE=.\ListClasses.h
# End Source File
# Begin Source File

SOURCE=.\ListTemplates.h
# End Source File
# Begin Source File

SOURCE=.\Master.h
# End Source File
# Begin Source File

SOURCE=.\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\Noise.h
# End Source File
# Begin Source File

SOURCE=.\PBuffer.h
# End Source File
# Begin Source File

SOURCE=.\PixelBuffer.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Texture.h
# End Source File
# Begin Source File

SOURCE=.\WndClass.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# End Group
# Begin Group "ROAM Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ROAMDiamond.cpp
# End Source File
# Begin Source File

SOURCE=.\ROAMDiamond.h
# End Source File
# Begin Source File

SOURCE=.\ROAMSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\ROAMSphere.h
# End Source File
# Begin Source File

SOURCE=.\ROAMTriangle.cpp
# End Source File
# Begin Source File

SOURCE=.\ROAMTriangle.h
# End Source File
# End Group
# Begin Group "Planetary Map Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\HeightMap.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightMap.h
# End Source File
# Begin Source File

SOURCE=.\PlanetaryMap.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetaryMap.h
# End Source File
# Begin Source File

SOURCE=.\SurfaceMap.cpp
# End Source File
# Begin Source File

SOURCE=.\SurfaceMap.h
# End Source File
# End Group
# Begin Group "Solar System Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Planet.cpp
# End Source File
# Begin Source File

SOURCE=.\Planet.h
# End Source File
# Begin Source File

SOURCE=.\PlanetInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\PlanetInfo.h
# End Source File
# Begin Source File

SOURCE=.\Star.cpp
# End Source File
# Begin Source File

SOURCE=.\Star.h
# End Source File
# Begin Source File

SOURCE=.\StarSystem.cpp
# End Source File
# Begin Source File

SOURCE=.\StarSystem.h
# End Source File
# End Group
# End Target
# End Project
