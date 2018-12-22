# Microsoft Developer Studio Project File - Name="SphinxMMOSPlayer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=SphinxMMOSPlayer - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "SphinxMMOSPlayer.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "SphinxMMOSPlayer.mak" CFG="SphinxMMOSPlayer - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "SphinxMMOSPlayer - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "SphinxMMOSPlayer - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "SphinxMMOSPlayer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\..\bin"
# PROP Intermediate_Dir "..\..\..\..\mak\player\release"
# PROP Ignore_Export_Lib 0
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
# ADD LINK32 /nologo /version:0.603 /subsystem:windows /pdb:"..\..\..\..\mak\SphinxMMOSPlayer.pdb" /debug /machine:I386
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "SphinxMMOSPlayer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\..\bin"
# PROP Intermediate_Dir "..\..\..\..\mak\player\debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /Gz /MDd /W3 /Gm /GX /ZI /Od /I "." /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /pdb:"..\..\..\..\mak\SphinxMMOSPlayerd.pdb" /debug /machine:I386 /out:"..\..\..\bin\SphinxMMOSPlayerd.exe" /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "SphinxMMOSPlayer - Win32 Release"
# Name "SphinxMMOSPlayer - Win32 Debug"
# Begin Group "app"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat;h"
# Begin Source File

SOURCE=.\atomos.cpp
# End Source File
# Begin Source File

SOURCE=.\atomos.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\sphinxmmos_includes.h
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayer.h
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayer.rc
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayerDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayerDoc.h
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayerMainFrame.cpp
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayerMainFrame.h
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayerView.cpp
# End Source File
# Begin Source File

SOURCE=.\SphinxMMOSPlayerView.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "res"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\SphinxMMOSPlayer.ico
# End Source File
# Begin Source File

SOURCE=.\res\SphinxMMOSPlayer.rc2
# End Source File
# Begin Source File

SOURCE=.\res\SphinxMMOSPlayerDoc.ico
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\toolbar1.bmp
# End Source File
# End Group
# Begin Group "patches"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=..\..\..\bin\media\patches\car01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\clipping01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\combustion01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\crossfade01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\envelopes01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\filter01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\flap00.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\flap01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\fm01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\helicopter01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\hierpatch01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\keyboard01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\muffler01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\noise01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\osc00.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\osc01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\osc02.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\patch00.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\patch01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\propeller01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\random01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\samples01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\settings.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\simple_patch.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\spaceship01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\submarine01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\test01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\turbo01.txt
# End Source File
# Begin Source File

SOURCE=..\..\..\bin\media\patches\wavetable01.txt
# End Source File
# End Group
# End Target
# End Project
