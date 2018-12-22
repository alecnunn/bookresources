# Microsoft Developer Studio Project File - Name="atomgaud" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=atomgaud - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "atomgaud.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "atomgaud.mak" CFG="atomgaud - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "atomgaud - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "atomgaud - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "atomgaud - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\bin"
# PROP Intermediate_Dir "..\..\..\..\mak\atomgaud\release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ATOMGAUD_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /Gz /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ATOMGAUD_EXPORTS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib /nologo /version:0.411 /dll /pdb:"..\..\..\..\mak\atomgaud.pdb" /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "atomgaud - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\bin"
# PROP Intermediate_Dir "..\..\..\..\mak\atomgaud\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ATOMGAUD_EXPORTS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "ATOMGAUD_EXPORTS" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib /nologo /version:0.411 /dll /pdb:"..\..\..\..\mak\atomgaudd.pdb" /debug /machine:I386 /out:"..\..\..\bin\atomgaudd.dll" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "atomgaud - Win32 Release"
# Name "atomgaud - Win32 Debug"
# Begin Group "src"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Source File

SOURCE=.\atomgaud.cpp
# End Source File
# Begin Source File

SOURCE=.\atomgaud.def
# End Source File
# Begin Source File

SOURCE=.\atomgaud.h
# End Source File
# Begin Source File

SOURCE=.\atomgaud.rc
# End Source File
# Begin Source File

SOURCE=.\atomgaud_defines.h
# End Source File
# Begin Source File

SOURCE=.\atomgaud_includes.h
# End Source File
# Begin Source File

SOURCE=.\GA303Filter.cpp
# End Source File
# Begin Source File

SOURCE=.\GA303Filter.h
# End Source File
# Begin Source File

SOURCE=.\GAAbs.cpp
# End Source File
# Begin Source File

SOURCE=.\GAAbs.h
# End Source File
# Begin Source File

SOURCE=.\GAAverager.cpp
# End Source File
# Begin Source File

SOURCE=.\GAAverager.h
# End Source File
# Begin Source File

SOURCE=.\GABalance.cpp
# End Source File
# Begin Source File

SOURCE=.\GABalance.h
# End Source File
# Begin Source File

SOURCE=.\GABipolarToUnipolar.cpp
# End Source File
# Begin Source File

SOURCE=.\GABipolarToUnipolar.h
# End Source File
# Begin Source File

SOURCE=.\GAButterworthFilter.cpp
# End Source File
# Begin Source File

SOURCE=.\GAButterworthFilter.h
# End Source File
# Begin Source File

SOURCE=.\GAButterworthLPF.cpp
# End Source File
# Begin Source File

SOURCE=.\GAButterworthLPF.h
# End Source File
# Begin Source File

SOURCE=.\GAClippedSineOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAClippedSineOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GAClipper.cpp
# End Source File
# Begin Source File

SOURCE=.\GAClipper.h
# End Source File
# Begin Source File

SOURCE=.\GAConfigurationReader.cpp
# End Source File
# Begin Source File

SOURCE=.\GAConfigurationReader.h
# End Source File
# Begin Source File

SOURCE=.\GAConnection.cpp
# End Source File
# Begin Source File

SOURCE=.\GAConnection.h
# End Source File
# Begin Source File

SOURCE=.\GAConnections.cpp
# End Source File
# Begin Source File

SOURCE=.\GAConnections.h
# End Source File
# Begin Source File

SOURCE=.\GAController.cpp
# End Source File
# Begin Source File

SOURCE=.\GAController.h
# End Source File
# Begin Source File

SOURCE=.\GACrossFade2.cpp
# End Source File
# Begin Source File

SOURCE=.\GACrossFade2.h
# End Source File
# Begin Source File

SOURCE=.\GACrossFade3.cpp
# End Source File
# Begin Source File

SOURCE=.\GACrossFade3.h
# End Source File
# Begin Source File

SOURCE=.\GACubic.cpp
# End Source File
# Begin Source File

SOURCE=.\GACubic.h
# End Source File
# Begin Source File

SOURCE=.\GADelay.cpp
# End Source File
# Begin Source File

SOURCE=.\GADelay.h
# End Source File
# Begin Source File

SOURCE=.\GADevice.cpp
# End Source File
# Begin Source File

SOURCE=.\GADevice.h
# End Source File
# Begin Source File

SOURCE=.\GADump.cpp
# End Source File
# Begin Source File

SOURCE=.\GADump.h
# End Source File
# Begin Source File

SOURCE=.\GAEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\GAEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\GAExpCurve.cpp
# End Source File
# Begin Source File

SOURCE=.\GAExpCurve.h
# End Source File
# Begin Source File

SOURCE=.\GAFile.cpp
# End Source File
# Begin Source File

SOURCE=.\GAFile.h
# End Source File
# Begin Source File

SOURCE=.\GAFunctionTable.cpp
# End Source File
# Begin Source File

SOURCE=.\GAFunctionTable.h
# End Source File
# Begin Source File

SOURCE=.\GAGain.cpp
# End Source File
# Begin Source File

SOURCE=.\GAGain.h
# End Source File
# Begin Source File

SOURCE=.\GAGaussEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\GAGaussEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\GAGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAGenerator.h
# End Source File
# Begin Source File

SOURCE=.\GAGlide.cpp
# End Source File
# Begin Source File

SOURCE=.\GAGlide.h
# End Source File
# Begin Source File

SOURCE=.\GAHalfCosineEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\GAHalfCosineEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\GAImpulseGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAImpulseGenerator.h
# End Source File
# Begin Source File

SOURCE=.\GAInterleaver.cpp
# End Source File
# Begin Source File

SOURCE=.\GAInterleaver.h
# End Source File
# Begin Source File

SOURCE=.\GAInverter.cpp
# End Source File
# Begin Source File

SOURCE=.\GAInverter.h
# End Source File
# Begin Source File

SOURCE=.\GAKeyboard.cpp
# End Source File
# Begin Source File

SOURCE=.\GAKeyboard.h
# End Source File
# Begin Source File

SOURCE=.\GAKeyboardController.cpp
# End Source File
# Begin Source File

SOURCE=.\GAKeyboardController.h
# End Source File
# Begin Source File

SOURCE=.\GALine.cpp
# End Source File
# Begin Source File

SOURCE=.\GALine.h
# End Source File
# Begin Source File

SOURCE=.\GAMath.cpp
# End Source File
# Begin Source File

SOURCE=.\GAMath.h
# End Source File
# Begin Source File

SOURCE=.\GAMixer.cpp
# End Source File
# Begin Source File

SOURCE=.\GAMixer.h
# End Source File
# Begin Source File

SOURCE=.\GAMixer2.cpp
# End Source File
# Begin Source File

SOURCE=.\GAMixer2.h
# End Source File
# Begin Source File

SOURCE=.\GAMixer3.cpp
# End Source File
# Begin Source File

SOURCE=.\GAMixer3.h
# End Source File
# Begin Source File

SOURCE=.\GAMixer4.cpp
# End Source File
# Begin Source File

SOURCE=.\GAMixer4.h
# End Source File
# Begin Source File

SOURCE=.\GAMixer6.cpp
# End Source File
# Begin Source File

SOURCE=.\GAMixer6.h
# End Source File
# Begin Source File

SOURCE=.\GANoiseGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\GANoiseGenerator.h
# End Source File
# Begin Source File

SOURCE=.\GANoiseMSineOsc.cpp
# End Source File
# Begin Source File

SOURCE=.\GANoiseMSineOsc.h
# End Source File
# Begin Source File

SOURCE=.\GANoiseParticle.cpp
# End Source File
# Begin Source File

SOURCE=.\GANoiseParticle.h
# End Source File
# Begin Source File

SOURCE=.\GANumPadController.cpp
# End Source File
# Begin Source File

SOURCE=.\GANumPadController.h
# End Source File
# Begin Source File

SOURCE=.\GAObject.cpp
# End Source File
# Begin Source File

SOURCE=.\GAObject.h
# End Source File
# Begin Source File

SOURCE=.\GAObjectReader.cpp
# End Source File
# Begin Source File

SOURCE=.\GAObjectReader.h
# End Source File
# Begin Source File

SOURCE=.\GAOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GAParser.cpp
# End Source File
# Begin Source File

SOURCE=.\GAParser.h
# End Source File
# Begin Source File

SOURCE=.\GAPatch.cpp
# End Source File
# Begin Source File

SOURCE=.\GAPatch.h
# End Source File
# Begin Source File

SOURCE=.\GAPatches.cpp
# End Source File
# Begin Source File

SOURCE=.\GAPatches.h
# End Source File
# Begin Source File

SOURCE=.\GAPhaser.cpp
# End Source File
# Begin Source File

SOURCE=.\GAPhaser.h
# End Source File
# Begin Source File

SOURCE=.\GAPin.cpp
# End Source File
# Begin Source File

SOURCE=.\GAPin.h
# End Source File
# Begin Source File

SOURCE=.\GAPins.cpp
# End Source File
# Begin Source File

SOURCE=.\GAPins.h
# End Source File
# Begin Source File

SOURCE=.\GAProcessor.cpp
# End Source File
# Begin Source File

SOURCE=.\GAProcessor.h
# End Source File
# Begin Source File

SOURCE=.\GAProcessors.cpp
# End Source File
# Begin Source File

SOURCE=.\GAProcessors.h
# End Source File
# Begin Source File

SOURCE=.\GAPulseOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAPulseOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GARampEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\GARampEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\GARandom.cpp
# End Source File
# Begin Source File

SOURCE=.\GARandom.h
# End Source File
# Begin Source File

SOURCE=.\GARandomGenerator.cpp
# End Source File
# Begin Source File

SOURCE=.\GARandomGenerator.h
# End Source File
# Begin Source File

SOURCE=.\GARenderer.cpp
# End Source File
# Begin Source File

SOURCE=.\GARenderer.h
# End Source File
# Begin Source File

SOURCE=.\GAResonator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAResonator.h
# End Source File
# Begin Source File

SOURCE=.\GARoarOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GARoarOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GASampleAndHold.cpp
# End Source File
# Begin Source File

SOURCE=.\GASampleAndHold.h
# End Source File
# Begin Source File

SOURCE=.\GASampleWriter.cpp
# End Source File
# Begin Source File

SOURCE=.\GASampleWriter.h
# End Source File
# Begin Source File

SOURCE=.\GASawtoothOscBLIT.cpp
# End Source File
# Begin Source File

SOURCE=.\GASawtoothOscBlit.h
# End Source File
# Begin Source File

SOURCE=.\GASawtoothOscDSF.cpp
# End Source File
# Begin Source File

SOURCE=.\GASawtoothOscDSF.h
# End Source File
# Begin Source File

SOURCE=.\GASawtoothOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GASawtoothOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GAScaleMapper.cpp
# End Source File
# Begin Source File

SOURCE=.\GAScaleMapper.h
# End Source File
# Begin Source File

SOURCE=.\GASelector.cpp
# End Source File
# Begin Source File

SOURCE=.\GASelector.h
# End Source File
# Begin Source File

SOURCE=.\GASelector2.cpp
# End Source File
# Begin Source File

SOURCE=.\GASelector2.h
# End Source File
# Begin Source File

SOURCE=.\GASequencer16.cpp
# End Source File
# Begin Source File

SOURCE=.\GASequencer16.h
# End Source File
# Begin Source File

SOURCE=.\GAShapeOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAShapeOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GASignal.cpp
# End Source File
# Begin Source File

SOURCE=.\GASignal.h
# End Source File
# Begin Source File

SOURCE=.\GASignalFormat.cpp
# End Source File
# Begin Source File

SOURCE=.\GASignalFormat.h
# End Source File
# Begin Source File

SOURCE=.\GASignalRandomizer.cpp
# End Source File
# Begin Source File

SOURCE=.\GASignalRandomizer.h
# End Source File
# Begin Source File

SOURCE=.\GASignals.cpp
# End Source File
# Begin Source File

SOURCE=.\GASignals.h
# End Source File
# Begin Source File

SOURCE=.\GASinCosOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GASinCosOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GASineOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GASineOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GASineXOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GASineXOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GASphereEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\GASphereEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\GASquareOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GASquareOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GAStream.cpp
# End Source File
# Begin Source File

SOURCE=.\GAStream.h
# End Source File
# Begin Source File

SOURCE=.\GASystem.cpp
# End Source File
# Begin Source File

SOURCE=.\GASystem.h
# End Source File
# Begin Source File

SOURCE=.\GAThreshold.cpp
# End Source File
# Begin Source File

SOURCE=.\GAThreshold.h
# End Source File
# Begin Source File

SOURCE=.\GAToken.cpp
# End Source File
# Begin Source File

SOURCE=.\GAToken.h
# End Source File
# Begin Source File

SOURCE=.\GATokenizer.cpp
# End Source File
# Begin Source File

SOURCE=.\GATokenizer.h
# End Source File
# Begin Source File

SOURCE=.\GATrapezEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\GATrapezEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\GATriangleEnvelope.cpp
# End Source File
# Begin Source File

SOURCE=.\GATriangleEnvelope.h
# End Source File
# Begin Source File

SOURCE=.\GATriangleOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GATriangleOscillator.h
# End Source File
# Begin Source File

SOURCE=.\GAWaveFile.cpp
# End Source File
# Begin Source File

SOURCE=.\GAWaveFile.h
# End Source File
# Begin Source File

SOURCE=.\GAWavetableOscillator.cpp
# End Source File
# Begin Source File

SOURCE=.\GAWavetableOscillator.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "portaudio"

# PROP Default_Filter "c++;c;h;"
# Begin Source File

SOURCE=.\portaudio\dsound_wrapper.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\portaudio\dsound_wrapper.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_dsound.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_host.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_lib.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_trace.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\portaudio\pa_trace.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\pablio.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\portaudio\pablio.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\portaudio.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\portaudio\portaudio.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\portaudio_includes.h
# End Source File
# Begin Source File

SOURCE=.\portaudio\ringbuffer.c
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# Begin Source File

SOURCE=.\portaudio\ringbuffer.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ReadMe.txt
# End Source File
# End Target
# End Project
