# Microsoft Developer Studio Project File - Name="gosh" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=gosh - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "gosh.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "gosh.mak" CFG="gosh - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "gosh - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "gosh - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "gosh - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GR /GX /O2 /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FTGL_LIBRARY_STATIC" /YX /FD /c
# SUBTRACT CPP /WX
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "gosh - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GR /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_CONSOLE" /D "_MBCS" /D "FTGL_LIBRARY_STATIC" /FR /FD /GZ /c
# SUBTRACT CPP /u
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none /nodefaultlib

!ENDIF 

# Begin Target

# Name "gosh - Win32 Release"
# Name "gosh - Win32 Debug"
# Begin Group "SGX Core Code"

# PROP Default_Filter ""
# Begin Group "SGX Utility Classes"

# PROP Default_Filter ""
# Begin Group "imagelib"

# PROP Default_Filter ""
# Begin Group "Image parsers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\utils\imagelib\bmpparser.cpp
# End Source File
# Begin Source File

SOURCE=sgx\utils\imagelib\bmpparser.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=sgx\utils\imagelib\imagedata.cpp
# End Source File
# Begin Source File

SOURCE=sgx\utils\imagelib\imagedata.hpp
# End Source File
# Begin Source File

SOURCE=sgx\utils\imagelib\imagelib.hpp
# End Source File
# Begin Source File

SOURCE=sgx\utils\imagelib\imageloader.cpp
# End Source File
# Begin Source File

SOURCE=sgx\utils\imagelib\imageloader.hpp
# End Source File
# Begin Source File

SOURCE=sgx\utils\imagelib\imageparser.cpp
# End Source File
# End Group
# End Group
# Begin Group "SGX Scripting Manager"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\scripting\scripting.hpp
# End Source File
# Begin Source File

SOURCE=sgx\scripting\scriptingengine.cpp
# End Source File
# Begin Source File

SOURCE=sgx\scripting\scriptingengine.hpp
# End Source File
# Begin Source File

SOURCE=sgx\scripting\scriptingenginestate.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\scripting\scriptingversion.hpp
# End Source File
# Begin Source File

SOURCE=sgx\scripting\scriptmanager.cpp
# End Source File
# Begin Source File

SOURCE=sgx\scripting\scriptmanager.hpp
# End Source File
# End Group
# Begin Group "SGX Video"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\video\video.hpp
# End Source File
# Begin Source File

SOURCE=sgx\video\videoformatmanager.cpp
# End Source File
# Begin Source File

SOURCE=sgx\video\videoformatmanager.hpp
# End Source File
# Begin Source File

SOURCE=sgx\video\videomanager.cpp
# End Source File
# Begin Source File

SOURCE=sgx\video\videomanager.hpp
# End Source File
# Begin Source File

SOURCE=sgx\video\videostream.cpp
# End Source File
# Begin Source File

SOURCE=sgx\video\videostream.hpp
# End Source File
# Begin Source File

SOURCE=sgx\video\videoversion.hpp
# End Source File
# End Group
# Begin Group "SGX Filesystem"

# PROP Default_Filter ""
# Begin Group "io"

# PROP Default_Filter ""
# Begin Group "fsioxml"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\filesystem\io\xml\fsioxmlnode.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\fsioxmlnode.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\fsioxmltree.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\fsioxmltree.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\xml.hpp
# End Source File
# End Group
# Begin Group "fsioSTL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\filesystem\stlio.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=sgx\filesystem\io\xml\baseio.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\fsiohardware.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\fsiohardware.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\fsiomath.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\fsiomath.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\io.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=sgx\filesystem\baseio.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\baseio.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\device.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\deviceblock.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\file.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\file.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\fileentry.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\filesystem\fileentry.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\filesys.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\filesys.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\filesysdata.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\filesysdata.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\filesystem.hpp
# End Source File
# End Group
# Begin Group "SGX Core"

# PROP Default_Filter ""
# Begin Group "Regex"

# PROP Default_Filter ""
# Begin Group "PCRE"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\chartables.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\config.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\get.c
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\internal.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\maketables.c
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\pcre.c
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\pcre.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\re.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\re.h
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\study.c
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\ucp.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\ucp.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\ucpinternal.h
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\ucptable.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\sgx\core\regex\pcre\ucptypetable.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Source File

SOURCE=sgx\core\regex\regex.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\regex\sgxwrapper.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\regex\sgxwrapper.hpp
# End Source File
# End Group
# Begin Group "CRC"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sgx\core\crc\crc.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\crc\crc32.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\crc\crc32.hpp
# End Source File
# End Group
# Begin Group "Mathematics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\math\constants.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\geometry.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\math.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\math.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\matrix43.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\matrix43.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\matrix44.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\matrix44.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\numerics.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\numerics.inl
# End Source File
# Begin Source File

SOURCE=sgx\core\math\sine.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\trigonometry.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\trigonometry.inl
# End Source File
# Begin Source File

SOURCE=sgx\core\types\typedefs.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\vector2.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\vector2.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\vector3.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\vector3.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\vector4.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\math\vector4.hpp
# End Source File
# End Group
# Begin Group "SGX Standard Libraries"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\sgxlib\commandline.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\commandline.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\ctype.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\qsort.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\random.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\random.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\sgxlib.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\states.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\string.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\string.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\sgxlib\utils.hpp
# End Source File
# End Group
# Begin Group "STL"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\stl\stl.hpp
# End Source File
# End Group
# Begin Group "Memory handlers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\memory\memlib.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\memory\memlib.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\memory\memory.hpp
# End Source File
# End Group
# Begin Group "Helpers"

# PROP Default_Filter ""
# Begin Group "Trace"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\helpers\globaltrace.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\helpers\outputtrace.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\helpers\trace.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\helpers\trace.hpp
# End Source File
# End Group
# Begin Group "Safety File"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\helpers\safety.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\helpers\safety.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=sgx\core\helpers\halt.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\helpers\helpers.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\helpers\memwatch.hpp
# End Source File
# End Group
# Begin Group "Time"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\time\gameclock.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\time\gameclock.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\time\systemclock.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\time\systemclock.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\time\time.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\time\timer.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\time\timer.hpp
# End Source File
# End Group
# Begin Group "Settings"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\settings\configuration.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\settings\configuration.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\settings\settings.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\settings\statistics.hpp
# End Source File
# End Group
# Begin Group "Types"

# PROP Default_Filter ""
# Begin Group "Platform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\types\platform\gamecube.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\platform\linux32.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\platform\ps2.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\platform\wintel32.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\platform\xbox.hpp
# End Source File
# End Group
# Begin Group "Compilers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\core\types\compiler\gnu.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\compiler\msvc.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\compiler\mwerks.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=sgx\core\types\alignment.hpp
# End Source File
# Begin Source File

SOURCE=.\sgx\core\types\casting.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\compiler.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\datatype.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\endian.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\platform.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\types\types.hpp
# End Source File
# End Group
# Begin Group "XML core"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sgx\core\xml\xml.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\xml\xmlbase.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\xml\xmlbase.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\xml\xmlnode.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\xmlparser.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\xmlparser.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\xmlreader.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\xmlreader.hpp
# End Source File
# Begin Source File

SOURCE=sgx\core\xml\xmltree.cpp
# End Source File
# Begin Source File

SOURCE=sgx\core\xml\xmltree.hpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\xmlwriter.cpp
# End Source File
# Begin Source File

SOURCE=sgx\filesystem\io\xml\xmlwriter.hpp
# End Source File
# End Group
# Begin Group "System"

# PROP Default_Filter ""
# Begin Source File

SOURCE=sgx\system\execute.cpp
# End Source File
# Begin Source File

SOURCE=sgx\system\execute.hpp
# End Source File
# Begin Source File

SOURCE=sgx\system\nullsystem.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=sgx\system\system.hpp
# End Source File
# Begin Source File

SOURCE=sgx\system\systemversion.hpp
# End Source File
# Begin Source File

SOURCE=sgx\system\yield.hpp
# End Source File
# End Group
# Begin Group "Hardware Platform"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sgx\core\platform\hwplatform.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\core\platform\hwplatform.hpp
# End Source File
# Begin Source File

SOURCE=.\sgx\core\platform\platform.hpp
# End Source File
# End Group
# Begin Group "Threading"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sgx\core\threading\thread.hpp
# End Source File
# Begin Source File

SOURCE=.\sgx\core\threading\threading.hpp
# End Source File
# Begin Source File

SOURCE=.\sgx\core\threading\threadmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\core\threading\threadmanager.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=sgx\core\core.hpp
# End Source File
# End Group
# End Group
# Begin Group "SGX Drivers"

# PROP Default_Filter ""
# Begin Group "WinTel32 Driver"

# PROP Default_Filter ""
# Begin Group "Timer (Wintel32)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\wintel32\core\time\win32systemclock.cpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\core\time\wintel32timer.cpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\core\time\wintel32timer.hpp
# End Source File
# End Group
# Begin Group "Win32Disc (Wintel32)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\wintel32\filesystem\filesystem.hpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\filesystem\win32device.cpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\filesystem\win32disc.hpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\filesystem\win32setup.cpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\filesystem\win32state.cpp
# End Source File
# End Group
# Begin Group "Platform (Wintel32)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\wintel32\core\platform\hwplatformwintel32.cpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\core\platform\hwplatformwintel32.hpp
# End Source File
# End Group
# Begin Group "System (Wintel32)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\wintel32\core\system\systemwintel32.cpp
# End Source File
# End Group
# Begin Group "Core (Wintel32)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\wintel32\core\helpers\wintrace.cpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\core\helpers\wintrace.hpp
# End Source File
# End Group
# Begin Group "Threading (Wintel32)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\wintel32\core\threading\threading.hpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\core\threading\wintel32threadmanager.cpp
# End Source File
# Begin Source File

SOURCE=.\drivers\wintel32\core\threading\wintel32threadmanager.hpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\drivers\wintel32\wintel32core.hpp
# End Source File
# End Group
# Begin Group "Null Driver"

# PROP Default_Filter ""
# Begin Group "Timer (Null)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\null\core\time\nullsystemclock.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\drivers\null\core\time\nulltimer.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\drivers\null\core\time\nulltimer.hpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Platform (Null)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\null\core\platform\hwplatformnull.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\drivers\null\core\platform\hwplatformnull.hpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "System (Null)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\null\core\system\systemnull.cpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Core (Null)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\null\core\helpers\nulltrace.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\drivers\null\core\helpers\nulltrace.hpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "Threading (Null)"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\null\core\threading\nullthreadmanager.cpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\drivers\null\core\threading\nullthreadmanager.hpp
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\drivers\null\core\threading\threading.hpp
# PROP Exclude_From_Build 1
# End Source File
# End Group
# End Group
# Begin Group "Other Platform Hardware"

# PROP Default_Filter ""
# Begin Group "Xbox"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\xbox\core\platform\hwplatformxbox.cpp
# End Source File
# End Group
# Begin Group "PS2"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\ps2\core\platform\hwplatformps2.cpp
# End Source File
# End Group
# Begin Group "Gamecube"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\drivers\gamecube\core\platform\hwplatformgamecube.cpp
# End Source File
# End Group
# End Group
# End Group
# Begin Group "SGX Libraries"

# PROP Default_Filter ""
# Begin Group "Lua Scripting"

# PROP Default_Filter ""
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libraries\lua\main\lapi.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lapi.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lcode.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lcode.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ldebug.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ldebug.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ldo.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ldo.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ldump.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lfunc.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lfunc.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lgc.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lgc.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\llex.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\llex.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\llimits.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lmem.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lmem.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lobject.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lobject.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lopcodes.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lopcodes.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lparser.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lparser.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lstate.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lstate.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lstring.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lstring.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ltable.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ltable.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ltests.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ltm.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\ltm.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lundump.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lundump.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lvm.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lvm.h
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lzio.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\main\lzio.h
# End Source File
# End Group
# Begin Group "AuxLib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libraries\lua\lib\lauxlib.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\lib\lbaselib.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\lib\ldblib.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\lib\liolib.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\lib\lmathlib.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\lib\loadlib.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\lib\lstrlib.c
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\lib\ltablib.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\libraries\lua\lua.hpp
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\luacore.hpp
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\luascriptingengine.cpp
# End Source File
# Begin Source File

SOURCE=.\libraries\lua\luascriptingengine.hpp
# End Source File
# Begin Source File

SOURCE=.\.\libraries\lua\luascriptstate.cpp
# End Source File
# End Group
# Begin Group "Expat XML Parser"

# PROP Default_Filter ""
# Begin Group "xmltok"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libraries\expat\xmltok\ascii.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\asciitab.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\dllmain.c
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\iasciitab.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\latin1tab.h
# End Source File
# Begin Source File

SOURCE=.\main\main.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\nametab.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\utf8tab.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmldef.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmlrole.c
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmlrole.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmltok.c
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmltok.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmltok_impl.c
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmltok_impl.h
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmltok\xmltok_ns.c
# PROP Exclude_From_Build 1
# End Source File
# End Group
# Begin Group "xmlparse"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\libraries\expat\xmlparse\xmlparse.c
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\xmlparse\xmlparse.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\libraries\expat\expat.cpp
# End Source File
# Begin Source File

SOURCE=.\libraries\expat\expat.hpp
# End Source File
# End Group
# End Group
# Begin Group "SGX Domains"

# PROP Default_Filter ""
# Begin Group "graphics"

# PROP Default_Filter ""
# Begin Group "color"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\sgx\domains\graphics\color\color.hpp
# End Source File
# Begin Source File

SOURCE=.\sgx\domains\graphics\color\colorconstants.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\domains\graphics\color\colorconversion.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\domains\graphics\color\colortypes.cpp
# End Source File
# Begin Source File

SOURCE=.\sgx\domains\graphics\color\colortypes.hpp
# End Source File
# End Group
# End Group
# End Group
# Begin Source File

SOURCE=.\main\main.cpp
# End Source File
# End Target
# End Project
