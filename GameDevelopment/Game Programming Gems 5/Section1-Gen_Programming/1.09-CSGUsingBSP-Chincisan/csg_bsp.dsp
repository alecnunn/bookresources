# Microsoft Developer Studio Project File - Name="csg_bsp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=csg_bsp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "csg_bsp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "csg_bsp.mak" CFG="csg_bsp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "csg_bsp - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "csg_bsp - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "csg_bsp - Win32 Release"

# PROP BASE Use_MFC 6
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
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /I "..\z-utils" /I "..\z-engine\\" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386 /nodefaultlib:"libc.lib" /out:"Release\csg_using_bsp.exe" /libpath:"..\lib\\"

!ELSEIF  "$(CFG)" == "csg_bsp - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\z-utils" /I "..\z-engine\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /FR /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"libc.lib" /nodefaultlib:"msvcrtd.lib" /nodefaultlib:"libcmt.lib" /out:"Debug\csg_using_bsp.exe" /pdbtype:sept /libpath:"..\lib\\"

!ENDIF 

# Begin Target

# Name "csg_bsp - Win32 Release"
# Name "csg_bsp - Win32 Debug"
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\bitmap3.bmp
# End Source File
# Begin Source File

SOURCE=.\res\crotate.cur
# End Source File
# Begin Source File

SOURCE=.\res\cur00001.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor1.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor2.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor3.cur
# End Source File
# Begin Source File

SOURCE=.\res\cursor4.cur
# End Source File
# Begin Source File

SOURCE=.\res\Toolbar.bmp
# End Source File
# Begin Source File

SOURCE=.\res\z_ed2.ico
# End Source File
# Begin Source File

SOURCE=.\res\z_ed2Doc.ico
# End Source File
# End Group
# Begin Group "CORE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\basegeom.h
# End Source File
# Begin Source File

SOURCE=.\basemath.cpp
# End Source File
# Begin Source File

SOURCE=.\basemath.h
# End Source File
# Begin Source File

SOURCE=.\baseos.h
# End Source File
# Begin Source File

SOURCE=.\basethread.h
# End Source File
# Begin Source File

SOURCE=.\baseutils.h
# End Source File
# Begin Source File

SOURCE=.\collections.h
# End Source File
# Begin Source File

SOURCE=.\TexRef.cpp
# End Source File
# Begin Source File

SOURCE=.\TexRef.h
# End Source File
# End Group
# Begin Group "BSP"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\BspTree.cpp
# End Source File
# Begin Source File

SOURCE=.\BspTree.h
# End Source File
# Begin Source File

SOURCE=.\Compiler.cpp
# End Source File
# Begin Source File

SOURCE=.\Compiler.h
# End Source File
# Begin Source File

SOURCE=.\MiniBsp.cpp
# End Source File
# Begin Source File

SOURCE=.\MiniBsp.h
# End Source File
# End Group
# Begin Group "GUI"

# PROP Default_Filter ""
# Begin Source File

SOURCE=".\csg_bsp.cpp"
# End Source File
# Begin Source File

SOURCE=".\csg_bsp.h"
# End Source File
# Begin Source File

SOURCE=".\csg_bsp.rc"
# End Source File
# Begin Source File

SOURCE=.\DlgHelp.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgHelp.h
# End Source File
# Begin Source File

SOURCE=.\DlgNewBrush.cpp
# End Source File
# Begin Source File

SOURCE=.\DlgNewBrush.h
# End Source File
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\z_ed2Doc.cpp
# End Source File
# Begin Source File

SOURCE=.\z_ed2Doc.h
# End Source File
# Begin Source File

SOURCE=.\z_ed2DocDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\z_ed2DocMk.cpp
# End Source File
# Begin Source File

SOURCE=.\z_ed2View.cpp
# End Source File
# Begin Source File

SOURCE=.\z_ed2View.h
# End Source File
# Begin Source File

SOURCE=.\z_ed3View.cpp
# End Source File
# Begin Source File

SOURCE=.\z_ed3View.h
# End Source File
# End Group
# Begin Group "DOC"

# PROP Default_Filter ""
# End Group
# Begin Group "CAMERA"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\camera.cpp
# End Source File
# Begin Source File

SOURCE=.\camera.h
# End Source File
# Begin Source File

SOURCE=.\Frustrum.cpp
# End Source File
# Begin Source File

SOURCE=.\Frustrum.h
# End Source File
# End Group
# Begin Group "SCENE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\brush.cpp
# End Source File
# Begin Source File

SOURCE=.\Brush.h
# End Source File
# Begin Source File

SOURCE=.\Poly.cpp
# End Source File
# Begin Source File

SOURCE=.\Poly.h
# End Source File
# Begin Source File

SOURCE=.\Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\Scene.h
# End Source File
# End Group
# Begin Group "UTILS"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\MMove.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# End Group
# End Target
# End Project
