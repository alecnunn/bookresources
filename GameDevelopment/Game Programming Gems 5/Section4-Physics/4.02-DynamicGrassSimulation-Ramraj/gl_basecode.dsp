# Microsoft Developer Studio Project File - Name="gl_basecode" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=gl_basecode - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gl_basecode.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gl_basecode.mak" CFG="gl_basecode - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gl_basecode - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "gl_basecode - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gl_basecode - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386 /out:"Release/Natural Effects.exe"

!ELSEIF  "$(CFG)" == "gl_basecode - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /out:"Debug/Natural Effects.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "gl_basecode - Win32 Release"
# Name "gl_basecode - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "basecode.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\basecode.cpp
# End Source File
# Begin Source File

SOURCE=.\dialog.rc
# End Source File
# Begin Source File

SOURCE=.\gl_wrap.cpp
# End Source File
# Begin Source File

SOURCE=.\win_wrap.cpp
# End Source File
# End Group
# Begin Group "effects.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\grass.cpp
# End Source File
# Begin Source File

SOURCE=.\leaves.cpp
# End Source File
# Begin Source File

SOURCE=.\water.cpp
# End Source File
# End Group
# Begin Group "libs.cpp"

# PROP Default_Filter ""
# Begin Group "mdl_loading.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cs_am3d.cpp
# End Source File
# Begin Source File

SOURCE=.\cs_ch3d.cpp
# End Source File
# Begin Source File

SOURCE=.\cs_kf3d.cpp
# End Source File
# Begin Source File

SOURCE=.\cs_sk3d.cpp
# End Source File
# End Group
# Begin Group "mth.cpp"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mth_angle.cpp
# End Source File
# Begin Source File

SOURCE=.\mth_matrix.cpp
# End Source File
# Begin Source File

SOURCE=.\mth_quaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\mth_spherical.cpp
# End Source File
# Begin Source File

SOURCE=.\mth_vector.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\particles.cpp
# End Source File
# Begin Source File

SOURCE=.\texture.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\demo.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "basecode.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gl_wrap.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\win_wrap.h
# End Source File
# End Group
# Begin Group "effects.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\grass.h
# End Source File
# Begin Source File

SOURCE=.\leaves.h
# End Source File
# Begin Source File

SOURCE=.\water.h
# End Source File
# End Group
# Begin Group "libs.h"

# PROP Default_Filter ""
# Begin Group "mdl_loading.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cs_am3d.h
# End Source File
# Begin Source File

SOURCE=.\cs_am3d_spec.h
# End Source File
# Begin Source File

SOURCE=.\cs_ch3d.h
# End Source File
# Begin Source File

SOURCE=.\cs_kf3d.h
# End Source File
# Begin Source File

SOURCE=.\cs_kf3d_spec.h
# End Source File
# Begin Source File

SOURCE=.\cs_sk3d.h
# End Source File
# Begin Source File

SOURCE=.\cs_sk3d_spec.h
# End Source File
# End Group
# Begin Group "mth.h"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mth_angle.h
# End Source File
# Begin Source File

SOURCE=.\mth_matrix.h
# End Source File
# Begin Source File

SOURCE=.\mth_quaternion.h
# End Source File
# Begin Source File

SOURCE=.\mth_spherical.h
# End Source File
# Begin Source File

SOURCE=.\mth_vector.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\particles.h
# End Source File
# Begin Source File

SOURCE=.\texture.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\demo.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon_32.ico
# End Source File
# Begin Source File

SOURCE=.\Screenshot.bmp
# End Source File
# End Group
# End Target
# End Project
