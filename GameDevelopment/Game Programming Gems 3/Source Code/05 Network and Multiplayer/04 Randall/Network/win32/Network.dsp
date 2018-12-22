# Microsoft Developer Studio Project File - Name="Network" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Network - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Network.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Network.mak" CFG="Network - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Network - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Network - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Network"
# PROP Scc_LocalPath ".."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Network - Win32 Release"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W4 /WX /GR /GX /O2 /I "../../" /I "../win32" /I "../../Thread/win32" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Yu"NetworkPCH.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "Network - Win32 Debug"

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
MTL=midl.exe
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W4 /WX /Gm /GR /GX /ZI /Od /I "../../" /I "../win32" /I "../../Thread/win32" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /Yu"NetworkPCH.h" /FD /GZ /c
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

# Name "Network - Win32 Release"
# Name "Network - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Address.cpp
# End Source File
# Begin Source File

SOURCE=..\Message.cpp
# End Source File
# Begin Source File

SOURCE=..\MessageRaw.cpp
# End Source File
# Begin Source File

SOURCE=.\NetworkPCH.cpp
# ADD CPP /Yc"NetworkPCH.h"
# End Source File
# Begin Source File

SOURCE=.\NetworkPlatform.cpp
# End Source File
# Begin Source File

SOURCE=.\push_socket.cpp
# End Source File
# Begin Source File

SOURCE=..\Sock.cpp
# End Source File
# Begin Source File

SOURCE=..\TcpClientSock.cpp
# End Source File
# Begin Source File

SOURCE=..\TcpMessageClient.cpp
# End Source File
# Begin Source File

SOURCE=..\TcpMessageServer.cpp
# End Source File
# Begin Source File

SOURCE=..\TcpPacketClient.cpp
# End Source File
# Begin Source File

SOURCE=..\TcpPacketServer.cpp
# End Source File
# Begin Source File

SOURCE=..\TcpServerSock.cpp
# End Source File
# Begin Source File

SOURCE=..\TcpSock.cpp
# End Source File
# Begin Source File

SOURCE=..\UdpSock.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\Address.h
# End Source File
# Begin Source File

SOURCE=..\BroadcastSock.h
# End Source File
# Begin Source File

SOURCE=..\ClientService.h
# End Source File
# Begin Source File

SOURCE=..\Message.h
# End Source File
# Begin Source File

SOURCE=..\MessageRaw.h
# End Source File
# Begin Source File

SOURCE=.\NetworkPCH.h
# End Source File
# Begin Source File

SOURCE=.\NetworkPlatform.h
# End Source File
# Begin Source File

SOURCE=..\push_socket.h
# End Source File
# Begin Source File

SOURCE=..\Sock.h
# End Source File
# Begin Source File

SOURCE=..\TcpClientSock.h
# End Source File
# Begin Source File

SOURCE=..\TcpMessageClient.h
# End Source File
# Begin Source File

SOURCE=..\TcpMessageServer.h
# End Source File
# Begin Source File

SOURCE=..\TcpPacketClient.h
# End Source File
# Begin Source File

SOURCE=..\TcpPacketServer.h
# End Source File
# Begin Source File

SOURCE=..\TcpServerSock.h
# End Source File
# Begin Source File

SOURCE=..\TcpSock.h
# End Source File
# Begin Source File

SOURCE=..\UdpSock.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\project.lnt
# End Source File
# End Target
# End Project
