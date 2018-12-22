# Microsoft Developer Studio Project File - Name="PanBox Edit" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=PanBox Edit - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "PanBox Edit.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "PanBox Edit.mak" CFG="PanBox Edit - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "PanBox Edit - Win32 Release" (basierend auf  "Win32 (x86) Application")
!MESSAGE "PanBox Edit - Win32 Debug" (basierend auf  "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "PanBox Edit - Win32 Release"

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

!ELSEIF  "$(CFG)" == "PanBox Edit - Win32 Debug"

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

# Name "PanBox Edit - Win32 Release"
# Name "PanBox Edit - Win32 Debug"
# Begin Group "Quellcodedateien"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\CApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\CEntity.cpp
# End Source File
# Begin Source File

SOURCE=.\CFile.cpp
# End Source File
# Begin Source File

SOURCE=.\CGeometry.cpp
# End Source File
# Begin Source File

SOURCE=.\CLevel.cpp
# End Source File
# Begin Source File

SOURCE=.\CLight.cpp
# End Source File
# Begin Source File

SOURCE=.\ComboBoxHelper.cpp
# End Source File
# Begin Source File

SOURCE=.\CPolygon.cpp
# End Source File
# Begin Source File

SOURCE=.\CPolymesh.cpp
# End Source File
# Begin Source File

SOURCE=.\CPortal.cpp
# End Source File
# Begin Source File

SOURCE=.\CSector.cpp
# End Source File
# Begin Source File

SOURCE=.\CSelectionBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\CSpawnPoint.cpp
# End Source File
# Begin Source File

SOURCE=.\CToolbar.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgCallbacks.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# End Group
# Begin Group "Header-Dateien"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\include\CApplication.h
# End Source File
# Begin Source File

SOURCE=.\include\CEntity.h
# End Source File
# Begin Source File

SOURCE=.\include\CFile.h
# End Source File
# Begin Source File

SOURCE=.\include\CGeometry.h
# End Source File
# Begin Source File

SOURCE=.\include\CLevel.h
# End Source File
# Begin Source File

SOURCE=.\include\CLevelObject.h
# End Source File
# Begin Source File

SOURCE=.\include\CLight.h
# End Source File
# Begin Source File

SOURCE=.\include\ComboBoxHelper.h
# End Source File
# Begin Source File

SOURCE=.\include\CPolygon.h
# End Source File
# Begin Source File

SOURCE=.\include\CPolymesh.h
# End Source File
# Begin Source File

SOURCE=.\include\CPortal.h
# End Source File
# Begin Source File

SOURCE=.\include\CSector.h
# End Source File
# Begin Source File

SOURCE=.\include\CSelectionBuffer.h
# End Source File
# Begin Source File

SOURCE=.\include\CSpawnPoint.h
# End Source File
# Begin Source File

SOURCE=.\include\CToolbar.h
# End Source File
# Begin Source File

SOURCE=.\include\DlgCallbacks.h
# End Source File
# Begin Source File

SOURCE=.\include\globals.h
# End Source File
# Begin Source File

SOURCE=.\include\gui.h
# End Source File
# Begin Source File

SOURCE=.\include\main.h
# End Source File
# Begin Source File

SOURCE=.\resources\resource.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFX.h
# End Source File
# Begin Source File

SOURCE=.\include\zfx3d.h
# End Source File
# Begin Source File

SOURCE=.\include\ZFXRenderDevice.h
# End Source File
# End Group
# Begin Group "Ressourcendateien"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\resources\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\resources\menu.rc
# End Source File
# Begin Source File

SOURCE=.\resources\toolbar1.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=.\Bugfixes.txt
# End Source File
# Begin Source File

SOURCE=.\include\Worklog.txt
# End Source File
# End Target
# End Project
