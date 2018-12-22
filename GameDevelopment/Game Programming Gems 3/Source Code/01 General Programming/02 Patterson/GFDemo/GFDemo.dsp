# Microsoft Developer Studio Project File - Name="GFDemo" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=GFDemo - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "GFDemo.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "GFDemo.mak" CFG="GFDemo - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "GFDemo - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "GFDemo - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "GFDemo - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "../" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"../GFBin/Win32/GFDemo.exe"

!ELSEIF  "$(CFG)" == "GFDemo - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /Zi /Od /I "../" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:no /debug /machine:I386 /out:"../GFBin/Win32/GFDemoD.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "GFDemo - Win32 Release"
# Name "GFDemo - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\AVLayerConfigObj_Game_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameEditor_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameHud_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameLose_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GamePause_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameViewer_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameWin_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_Intro_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_StartMenu_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_SystemConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_Game_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameEditor_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameHud_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameLose_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GamePause_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameViewer_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameWin_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_Intro_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_StartMenu_t.cpp
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_SystemConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\GameApp_t.cpp
# End Source File
# Begin Source File

SOURCE=.\GameFactoryImpl_t.cpp
# End Source File
# Begin Source File

SOURCE=.\GameState_t.cpp
# End Source File
# Begin Source File

SOURCE=.\GFDemo.cpp
# End Source File
# Begin Source File

SOURCE=.\GFDemoResources.rc
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_Game_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameEditor_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameLose_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GamePause_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameViewer_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameWin_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_Intro_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_StartMenu_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_SystemConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_Game_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameEditor_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameLose_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GamePause_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameViewer_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameWin_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_Intro_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_StartMenu_t.cpp
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_SystemConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_Game_t.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_GameConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_Intro_t.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_SystemConfig_t.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskObj_ChangeDriver_t.cpp
# End Source File
# Begin Source File

SOURCE=.\TaskObj_StartApp_t.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\AVLayerConfigObj_Game_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameEditor_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameHud_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameLose_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GamePause_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameViewer_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_GameWin_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_Intro_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_StartMenu_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerConfigObj_SystemConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_Game_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameEditor_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameHud_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameLose_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GamePause_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameViewer_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_GameWin_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_Intro_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_StartMenu_t.h
# End Source File
# Begin Source File

SOURCE=.\AVLayerObj_SystemConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\GameApp_t.h
# End Source File
# Begin Source File

SOURCE=.\GameCollectionIDs.h
# End Source File
# Begin Source File

SOURCE=.\GameConstants.h
# End Source File
# Begin Source File

SOURCE=.\GameFactoryIDs.h
# End Source File
# Begin Source File

SOURCE=.\GameFactoryImpl_t.h
# End Source File
# Begin Source File

SOURCE=.\GameInstanceIDs.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_Game_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameEditor_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameLose_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GamePause_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameViewer_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_GameWin_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_Intro_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_StartMenu_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerConfigObj_SystemConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_Game_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameEditor_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameLose_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GamePause_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameViewer_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_GameWin_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_Intro_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_StartMenu_t.h
# End Source File
# Begin Source File

SOURCE=.\LogicLayerObj_SystemConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_Game_t.h
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_GameConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_Intro_t.h
# End Source File
# Begin Source File

SOURCE=.\TaskConfigObj_SystemConfig_t.h
# End Source File
# Begin Source File

SOURCE=.\TaskObj_ChangeDriver_t.h
# End Source File
# Begin Source File

SOURCE=.\TaskObj_StartApp_t.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\GFDemo.ICO
# End Source File
# Begin Source File

SOURCE=.\SMALL.ICO
# End Source File
# End Group
# Begin Group "GFIncludes"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\GFInclude\GFTypes.h
# End Source File
# End Group
# Begin Group "GFInclude/Framework"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\GFInclude\Framework\AVLayer_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\AVLayerCommand_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\AVLayerConfig_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\FramePlayer_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\LogicLayer_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\LogicLayerCommand_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\LogicLayerConfig_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\Task_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\TaskCommand_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Framework\TaskConfig_t.h
# End Source File
# End Group
# Begin Group "GFInclude/Systems"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\GFInclude\Systems\AudioSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\ErrorSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\FactorySys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\FrameSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\GameFactory_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\InputSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\LogSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\NetworkSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\ProcessInfo_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\ResourceSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\Systems_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\TaskSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\TimeSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\VisualSys_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Systems\WindowSys_t.h
# End Source File
# End Group
# Begin Group "GFInclude/Objects"

# PROP Default_Filter "*.h"
# Begin Source File

SOURCE=..\GFInclude\Objects\AudioSample_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Objects\Box2D_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Objects\Camera2D_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Objects\Camera3D_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Objects\MenuBox2D_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Objects\Sound_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Objects\TextBox2D_t.h
# End Source File
# Begin Source File

SOURCE=..\GFInclude\Objects\Texture_t.h
# End Source File
# End Group
# End Target
# End Project
