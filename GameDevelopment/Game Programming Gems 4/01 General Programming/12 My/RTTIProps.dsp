# Microsoft Developer Studio Project File - Name="RTTIProps" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=RTTIProps - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "RTTIProps.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "RTTIProps.mak" CFG="RTTIProps - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "RTTIProps - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "RTTIProps - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "RTTIProps - Win32 Release"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I ".\core" /I ".\dialogs" /I ".\engine" /I ".\PropSystem" /I ".\rendering" /I ".\editor" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "NDEBUG"
# ADD RSC /l 0x40c /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 gridctrl.lib d3d8.lib d3dx8.lib /nologo /subsystem:windows /machine:I386 /libpath:"./gridctrl/release"

!ELSEIF  "$(CFG)" == "RTTIProps - Win32 Debug"

# PROP BASE Use_MFC 5
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I ".\core" /I ".\dialogs" /I ".\engine" /I ".\PropSystem" /I ".\rendering" /I ".\editor" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x40c /d "_DEBUG"
# ADD RSC /l 0x40c /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 gridctrld.lib d3d8.lib d3dx8.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"./gridctrl/debug"

!ENDIF 

# Begin Target

# Name "RTTIProps - Win32 Release"
# Name "RTTIProps - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\res\RTTIProps.ico
# End Source File
# Begin Source File

SOURCE=.\RTTIProps.rc
# End Source File
# Begin Source File

SOURCE=.\res\RTTIProps.rc2
# End Source File
# Begin Source File

SOURCE=.\res\RTTIPropsDoc.ico
# End Source File
# End Group
# Begin Group "Core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Core\BasicTypes.h
# End Source File
# Begin Source File

SOURCE=.\Core\Defines.h
# End Source File
# Begin Source File

SOURCE=.\Core\Dir.h
# End Source File
# Begin Source File

SOURCE=.\Core\Dir.inl
# End Source File
# Begin Source File

SOURCE=.\Core\DirC.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\DirC.h
# End Source File
# Begin Source File

SOURCE=.\Core\DirC.inl
# End Source File
# Begin Source File

SOURCE=.\Core\ErrorAfxMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\ErrorAfxMsg.h
# End Source File
# Begin Source File

SOURCE=.\Core\File.h
# End Source File
# Begin Source File

SOURCE=.\Core\FileC.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\FileC.h
# End Source File
# Begin Source File

SOURCE=.\Core\FileC.inl
# End Source File
# Begin Source File

SOURCE=.\Core\FindFile.h
# End Source File
# Begin Source File

SOURCE=.\Core\FindFileC.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\FindFileC.h
# End Source File
# Begin Source File

SOURCE=.\Core\FindFileC.inl
# End Source File
# Begin Source File

SOURCE=.\Core\Logger.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Logger.h
# End Source File
# Begin Source File

SOURCE=.\Core\Logger.inl
# End Source File
# Begin Source File

SOURCE=.\Core\LogHelper.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MemC.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MemC.h
# End Source File
# Begin Source File

SOURCE=.\Core\MemC.inl
# End Source File
# Begin Source File

SOURCE=.\Core\MyAssert.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\MyAssert.h
# End Source File
# Begin Source File

SOURCE=.\Core\Quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\Quaternion.h
# End Source File
# Begin Source File

SOURCE=.\Core\Quaternion.inl
# End Source File
# Begin Source File

SOURCE=.\Core\RefCount.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\RefCount.h
# End Source File
# Begin Source File

SOURCE=.\Core\RefCount.inl
# End Source File
# Begin Source File

SOURCE=.\Core\RTTI.h
# End Source File
# Begin Source File

SOURCE=.\Core\SmartPtr.h
# End Source File
# Begin Source File

SOURCE=.\Core\SmartPtr.inl
# End Source File
# Begin Source File

SOURCE=.\Core\StdString.h
# End Source File
# Begin Source File

SOURCE=.\Core\StlIncludes.h
# End Source File
# Begin Source File

SOURCE=.\Core\StringC.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\StringC.h
# End Source File
# Begin Source File

SOURCE=.\Core\StringC.inl
# End Source File
# Begin Source File

SOURCE=.\Core\TexIoTGA.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\TexIoTGA.h
# End Source File
# Begin Source File

SOURCE=.\Core\TextureIO.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\TextureIO.h
# End Source File
# Begin Source File

SOURCE=.\Core\TSet.h
# End Source File
# Begin Source File

SOURCE=.\Core\TSet.inl
# End Source File
# Begin Source File

SOURCE=.\Core\VectorMatrix.cpp
# End Source File
# Begin Source File

SOURCE=.\Core\VectorMatrix.h
# End Source File
# Begin Source File

SOURCE=.\Core\VectorMatrix.inl
# End Source File
# End Group
# Begin Group "MFC"

# PROP Default_Filter ""
# Begin Group "Dialogs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Dialogs\DlgChooseMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgChooseMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgChooseTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgChooseTexture.h
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgEditBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgEditBox.h
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgEditCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgEditCamera.h
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgEditSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgEditSphere.h
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgItemName.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgItemName.h
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgRotation.cpp
# End Source File
# Begin Source File

SOURCE=.\Dialogs\DlgRotation.h
# End Source File
# End Group
# Begin Group "Views"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\3DView.cpp
# End Source File
# Begin Source File

SOURCE=.\3DView.h
# End Source File
# Begin Source File

SOURCE=.\CommandView.cpp
# End Source File
# Begin Source File

SOURCE=.\CommandView.h
# End Source File
# Begin Source File

SOURCE=.\LogReportView.cpp
# End Source File
# Begin Source File

SOURCE=.\LogReportView.h
# End Source File
# Begin Source File

SOURCE=.\PropView.cpp
# End Source File
# Begin Source File

SOURCE=.\PropView.h
# End Source File
# Begin Source File

SOURCE=.\SceneView.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneView.h
# End Source File
# End Group
# Begin Group "Controls"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ColoredStatusBar.cpp
# End Source File
# Begin Source File

SOURCE=.\ColoredStatusBar.h
# End Source File
# Begin Source File

SOURCE=.\FixedSplitter.cpp
# End Source File
# Begin Source File

SOURCE=.\FixedSplitter.h
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeCtrlEx.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ChildFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\ChildFrm.h
# End Source File
# Begin Source File

SOURCE=.\CustomMsgs.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\RTTIProps.cpp
# End Source File
# Begin Source File

SOURCE=.\RTTIProps.h
# End Source File
# Begin Source File

SOURCE=.\RTTIPropsDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\RTTIPropsDoc.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# Begin Group "PropSystem"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\PropSystem\ExtraData.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\ExtraData.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\ExtraProp.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\ExtraProp.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\ExtraProp.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\LevelFiles.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\LevelFiles.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\LevelFiles.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\LinkLoad.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\LinkLoad.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Persistent.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Persistent.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Persistent.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropDescMap.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropDescMap.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropDescMap.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Properties.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Properties.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Properties.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Property.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Property.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\Property.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropList.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropList.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropList.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropListLoad.cpp
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropListLoad.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\PropListLoad.inl
# End Source File
# Begin Source File

SOURCE=.\PropSystem\RTTIMap.h
# End Source File
# Begin Source File

SOURCE=.\PropSystem\RTTIMap.inl
# End Source File
# End Group
# Begin Group "Engine"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Engine\Engine.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\Engine.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineBox.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineBox.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineCamera.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineCamera.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineMaterial.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineMesh.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineMesh.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineMesh.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineNode.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineNode.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineObj.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineObj.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineObj.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineObjects.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineSphere.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineSphere.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineTexture.h
# End Source File
# Begin Source File

SOURCE=.\Engine\EngineTexture.inl
# End Source File
# Begin Source File

SOURCE=.\Engine\ListObj.cpp
# End Source File
# Begin Source File

SOURCE=.\Engine\ListObj.h
# End Source File
# Begin Source File

SOURCE=.\Engine\ListObj.inl
# End Source File
# End Group
# Begin Group "Rendering"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Rendering\IBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\IBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\IBuffer.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\IBufferDX8.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\IBufferDX8.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\IBufferDX8.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\MeshDX8.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\MeshDX8.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\MeshDX8.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\Renderer.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\Renderer.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\Renderer.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\RendererDX8.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\RendererDX8.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\RendererDX8.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\SwapChain.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\SwapChain.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\SwapChain.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\SwapChainDX8.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\SwapChainDX8.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\SwapChainDX8.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\TextureDX8.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\TextureDX8.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\TextureDX8.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\VBuffer.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\VBuffer.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\VBuffer.inl
# End Source File
# Begin Source File

SOURCE=.\Rendering\VBufferDX8.cpp
# End Source File
# Begin Source File

SOURCE=.\Rendering\VBufferDX8.h
# End Source File
# Begin Source File

SOURCE=.\Rendering\VBufferDX8.inl
# End Source File
# End Group
# Begin Group "Editor"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Editor\EditorBox.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorBox.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorBox.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorCamera.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorCamera.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorCamera.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorFolder.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorFolder.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorFolder.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorMaterial.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorMaterial.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorMaterial.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorNode.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorNode.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorNode.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorObj.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorObj.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorObj.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorObjects.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorScene.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorScene.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorScene.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorSceneItem.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorSceneItem.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorSceneItem.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorSphere.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorSphere.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorSphere.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorTexture.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorTexture.h
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorTexture.inl
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\EditorTool.h
# End Source File
# Begin Source File

SOURCE=.\Editor\ImgEnum.h
# End Source File
# Begin Source File

SOURCE=.\Editor\ImgMenu.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\ImgMenu.h
# End Source File
# Begin Source File

SOURCE=.\Editor\MenuEnum.h
# End Source File
# Begin Source File

SOURCE=.\Editor\MoveCameraTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\MoveCameraTool.h
# End Source File
# Begin Source File

SOURCE=.\Editor\MoveSelectionTool.cpp
# End Source File
# Begin Source File

SOURCE=.\Editor\MoveSelectionTool.h
# End Source File
# End Group
# End Target
# End Project
