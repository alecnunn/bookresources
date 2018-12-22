# Microsoft Developer Studio Project File - Name="AudioScriptingExample" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=AudioScriptingExample - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "AudioScriptingExample.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "AudioScriptingExample.mak" CFG="AudioScriptingExample - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "AudioScriptingExample - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "AudioScriptingExample - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "AudioScriptingExample - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "oggvorbis\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 ogg_static.lib vorbis_static.lib vorbisfile_static.lib winmm.lib dsound.lib dxerr9.lib dxguid.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib commctrl.lib /nologo /subsystem:windows /machine:I386 /out:"Bin/AudioScriptingExample.exe" /libpath:"oggvorbis\lib"

!ELSEIF  "$(CFG)" == "AudioScriptingExample - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "AudioScriptingExample___Win32_Debug"
# PROP BASE Intermediate_Dir "AudioScriptingExample___Win32_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "oggvorbis\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FAs /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ogg_static_d.lib vorbis_static_d.lib vorbisfile_static_d.lib winmm.lib dsound.lib dxerr9.lib dxguid.lib kernel32.lib user32.lib gdi32.lib comdlg32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Bin/AudioScriptingExample.exe" /pdbtype:sept /libpath:"oggvorbis\lib"

!ENDIF 

# Begin Target

# Name "AudioScriptingExample - Win32 Release"
# Name "AudioScriptingExample - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\AudioScriptingExample.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Audio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Audio.h
# End Source File
# Begin Source File

SOURCE=.\AudioManager.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioManager.h
# End Source File
# Begin Source File

SOURCE=.\AudioTag.cpp
# End Source File
# Begin Source File

SOURCE=.\AudioTag.h
# End Source File
# Begin Source File

SOURCE=.\Camera.h
# End Source File
# Begin Source File

SOURCE=.\Music.h
# End Source File
# Begin Source File

SOURCE=.\MusicOggVorbis.cpp
# End Source File
# Begin Source File

SOURCE=.\MusicOggVorbis.h
# End Source File
# Begin Source File

SOURCE=.\OggVorbisFile.cpp
# End Source File
# Begin Source File

SOURCE=.\OggVorbisFile.h
# End Source File
# Begin Source File

SOURCE=.\Sound.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound.h
# End Source File
# Begin Source File

SOURCE=.\Sound3D.cpp
# End Source File
# Begin Source File

SOURCE=.\Sound3D.h
# End Source File
# Begin Source File

SOURCE=.\WaveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveFile.h
# End Source File
# Begin Source File

SOURCE=.\WaveFileFactory.cpp
# End Source File
# Begin Source File

SOURCE=.\WaveFileFactory.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\AudioScriptingExample.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\WorldObject.h
# End Source File
# End Target
# End Project
