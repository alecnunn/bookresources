# Microsoft Developer Studio Project File - Name="Pandoras Legacy" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=Pandoras Legacy - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "Pandoras Legacy.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "Pandoras Legacy.mak" CFG="Pandoras Legacy - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "Pandoras Legacy - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "Pandoras Legacy - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Pandoras Legacy - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "Pandoras Legacy - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "Pandoras Legacy - Win32 Release"
# Name "Pandoras Legacy - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CGame.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameBulletList.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameCharacter.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameLight.cpp
# End Source File
# Begin Source File

SOURCE=.\CGamePlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\CGamePortal.cpp
# End Source File
# Begin Source File

SOURCE=.\CGameSector.cpp
# End Source File
# Begin Source File

SOURCE=.\CShadowVolume.cpp
# End Source File
# Begin Source File

SOURCE=.\CZFXModel.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\CGame.h
# End Source File
# Begin Source File

SOURCE=.\include\CGameBulletList.h
# End Source File
# Begin Source File

SOURCE=.\include\CGameCharacter.h
# End Source File
# Begin Source File

SOURCE=.\include\CGameEntity.h
# End Source File
# Begin Source File

SOURCE=.\include\CGameLevel.h
# End Source File
# Begin Source File

SOURCE=.\include\CGameLight.h
# End Source File
# Begin Source File

SOURCE=.\include\CGamePlayer.h
# End Source File
# Begin Source File

SOURCE=.\include\CGamePortal.h
# End Source File
# Begin Source File

SOURCE=.\include\CGameSector.h
# End Source File
# Begin Source File

SOURCE=.\include\CShadowVolume.h
# End Source File
# Begin Source File

SOURCE=.\include\CZFXModel.h
# End Source File
# Begin Source File

SOURCE=.\include\main.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFX.h
# End Source File
# Begin Source File

SOURCE=.\include\zfx3d.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXAudioDevice.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXGeneral.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXInput.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXInputDevice.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXMCAHornet.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXMCAircraft.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXMCEgo.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXMCFree.h
# End Source File
# Begin Source File

SOURCE=.\include\zfxModell.h
# End Source File
# Begin Source File

SOURCE=.\include\zfxModellCBF.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXModelStructs.H
# End Source File
# Begin Source File

SOURCE=.\include\ZFXMovementController.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXNetwork.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXNetworkDevice.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXRenderDevice.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXRenderer.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXSTRUKTUREN.H
# End Source File
# Begin Source File

SOURCE=.\include\ZFXTimer.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Skript1.rc
# End Source File
# End Group
# End Target
# End Project
