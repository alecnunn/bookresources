# Microsoft Developer Studio Project File - Name="serializelib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=serializelib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "serializelib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "serializelib.mak" CFG="serializelib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "serializelib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "serializelib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "serializelib - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\..\stl" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "serializelib - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\stl" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ENDIF 

# Begin Target

# Name "serializelib - Win32 Release"
# Name "serializelib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\src\basetypes.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\buffer.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\byteorder.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\serdefs.cpp
# End Source File
# Begin Source File

SOURCE=..\..\src\serializer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\src\basetypes.h
# End Source File
# Begin Source File

SOURCE=..\..\src\buffer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\byteorder.h
# End Source File
# Begin Source File

SOURCE=..\..\src\keyedserializer.h
# End Source File
# Begin Source File

SOURCE=..\..\src\misc.h
# End Source File
# Begin Source File

SOURCE=..\..\src\platform.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_deque.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_hashmap.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_hashset.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_list.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_map.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_pair.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_set.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_stl.h
# End Source File
# Begin Source File

SOURCE=..\..\src\ser_vector.h
# End Source File
# Begin Source File

SOURCE=..\..\src\serdefs.h
# End Source File
# Begin Source File

SOURCE=..\..\src\serializer.h
# End Source File
# End Group
# End Target
# End Project
