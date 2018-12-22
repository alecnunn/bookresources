# Microsoft Developer Studio Project File - Name="BiquadTest" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=BiquadTest - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "BiquadTest.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "BiquadTest.mak" CFG="BiquadTest - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "BiquadTest - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "BiquadTest - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "BiquadTest - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"../BiquadTest.exe"

!ELSEIF  "$(CFG)" == "BiquadTest - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"../BiquadTest.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "BiquadTest - Win32 Release"
# Name "BiquadTest - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\BiquadTest.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\PortAudioDevice.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Add.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_BandpassFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_BiquadFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Filter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_HighpassFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_ImpulseOscillator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_LowPassFilter.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Multiply.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Oscillator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_SineOscillator.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_WhiteNoise.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\UnitGen.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\ggsynth.h
# End Source File
# Begin Source File

SOURCE=..\..\src\PortAudioDevice.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Add.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_BandpassFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_BiquadFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Filter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_HighpassFilter.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_ImpulseOscillator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Multiply.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_Oscillator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_RedNoise.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_SineOscillator.h
# End Source File
# Begin Source File

SOURCE=..\..\src\Unit_WhiteNoise.h
# End Source File
# Begin Source File

SOURCE=..\..\src\UnitGen.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "PortAudio"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\..\src\pa_host.h
# End Source File
# Begin Source File

SOURCE=..\..\src\pa_lib.c
# End Source File
# Begin Source File

SOURCE=..\..\src\pa_trace.h
# End Source File
# Begin Source File

SOURCE=..\..\src\pa_win_wmme.c
# End Source File
# Begin Source File

SOURCE=..\..\src\portaudio.h
# End Source File
# End Group
# End Target
# End Project
