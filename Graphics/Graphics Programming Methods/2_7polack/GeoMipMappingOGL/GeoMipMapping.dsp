# Microsoft Developer Studio Project File - Name="GeoMipMapping" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GeoMipMapping - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GeoMipMapping.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GeoMipMapping.mak" CFG="GeoMipMapping - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GeoMipMapping - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GeoMipMapping - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GeoMipMapping - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "GeoMipMapping - Win32 Debug"

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
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GeoMipMapping - Win32 Release"
# Name "GeoMipMapping - Win32 Debug"
# Begin Group "Demo"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DemoApp.cpp
# End Source File
# Begin Source File

SOURCE=.\DemoApp.h
# End Source File
# End Group
# Begin Group "Terrain"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GeoMipMapping.cpp
# End Source File
# Begin Source File

SOURCE=.\GeoMipMapping.h
# End Source File
# Begin Source File

SOURCE=.\HeightMap.cpp
# End Source File
# Begin Source File

SOURCE=.\HeightMap.h
# End Source File
# End Group
# Begin Group "Framework"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Framework\Application.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework\Application.h
# End Source File
# Begin Source File

SOURCE=.\Framework\Camera.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Framework\EventHandler.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework\EventHandler.h
# End Source File
# Begin Source File

SOURCE=.\Framework\FontEngine.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework\FontEngine.h
# End Source File
# Begin Source File

SOURCE=.\Framework\glext.h
# End Source File
# Begin Source File

SOURCE=.\Framework\GLExtensions.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework\GLExtensions.h
# End Source File
# Begin Source File

SOURCE=.\Framework\Log.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework\Log.h
# End Source File
# Begin Source File

SOURCE=.\Framework\MessageHandler.h
# End Source File
# Begin Source File

SOURCE=.\Framework\Texture.cpp
# End Source File
# Begin Source File

SOURCE=.\Framework\Texture.h
# End Source File
# Begin Source File

SOURCE=.\Framework\Timer.h
# End Source File
# End Group
# Begin Group "MathLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MathLib\Matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\MathLib\Matrix.h
# End Source File
# Begin Source File

SOURCE=.\MathLib\Vector.cpp
# End Source File
# Begin Source File

SOURCE=.\MathLib\Vector.h
# End Source File
# End Group
# End Target
# End Project
