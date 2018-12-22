# Microsoft Developer Studio Project File - Name="simple_float_pbuffer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=simple_float_pbuffer - WIN32 DEBUG
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "simple_float_pbuffer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "simple_float_pbuffer.mak" CFG="simple_float_pbuffer - WIN32 DEBUG"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "simple_float_pbuffer - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "simple_float_pbuffer - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "mjkpc"
# PROP Scc_LocalPath "."
CPP=xicl6.exe
RSC=rc.exe

!IF  "$(CFG)" == "simple_float_pbuffer - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ""
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /Zi /O2 /Ob2 /I "..\..\..\include\glh" /I "..\..\..\include\shared" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR"" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib nvparse.lib libpng.lib cg.lib cgGL.lib paramgl.lib nvIO.lib vfw32.lib /nologo /subsystem:console /profile /debug /machine:I386 /out:"softshadows.exe" /libpath:"../../../lib/Release" /libpath:"../../../../Common/lib/Release" /fixed:no

!ELSEIF  "$(CFG)" == "simple_float_pbuffer - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ""
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\..\..\include\glh" /I "..\..\..\include\shared" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=xilink6.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib nvparse.lib libpng.lib cg.lib cgGL.lib paramgl.lib nvIO.lib vfw32.lib /nologo /subsystem:console /debug /machine:I386 /out:"dbg_softshadows.exe" /pdbtype:sept /libpath:"../../../lib/Debug" /libpath:"../../../../Common/lib/Debug"

!ENDIF 

# Begin Target

# Name "simple_float_pbuffer - Win32 Release"
# Name "simple_float_pbuffer - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\shared\data_path.cpp
# End Source File
# Begin Source File

SOURCE=.\mistynight.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\MovieMaker.cpp
# End Source File
# Begin Source File

SOURCE=..\..\shared\nv_dds.cpp
# End Source File
# Begin Source File

SOURCE="..\MistKnight - faster\shadobject.cpp"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\..\..\include\shared\MovieMaker.h
# End Source File
# Begin Source File

SOURCE=.\shadobject.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# Begin Group "Vertex Shaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cg_shimmerVP.cg

!IF  "$(CFG)" == "simple_float_pbuffer - Win32 Release"

# Begin Custom Build
InputPath=.\cg_shimmerVP.cg
InputName=cg_shimmerVP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"  "$(InputPath)" -o "$(InputName).vp" -profile vp30

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_float_pbuffer - Win32 Debug"

# Begin Custom Build
InputPath=.\cg_shimmerVP.cg
InputName=cg_shimmerVP

"$(InputName).vp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cgc "$(InputPath)" -o "$(InputName).vp" -profile vp20

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Group "Pixel Shaders"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\cg_final_FP.cg

!IF  "$(CFG)" == "simple_float_pbuffer - Win32 Release"

# Begin Custom Build
InputPath=.\cg_final_FP.cg
InputName=cg_final_FP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_float_pbuffer - Win32 Debug"

# Begin Custom Build
InputPath=.\cg_final_FP.cg
InputName=cg_final_FP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cg_fog_FP.cg

!IF  "$(CFG)" == "simple_float_pbuffer - Win32 Release"

# Begin Custom Build
InputPath=.\cg_fog_FP.cg
InputName=cg_fog_FP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_float_pbuffer - Win32 Debug"

# Begin Custom Build
InputPath=.\cg_fog_FP.cg
InputName=cg_fog_FP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cg_shad_FP.cg

!IF  "$(CFG)" == "simple_float_pbuffer - Win32 Release"

# Begin Custom Build
InputPath=.\cg_shad_FP.cg
InputName=cg_shad_FP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_float_pbuffer - Win32 Debug"

# Begin Custom Build
InputPath=.\cg_shad_FP.cg
InputName=cg_shad_FP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\cg_shimmer_dtestFP.cg

!IF  "$(CFG)" == "simple_float_pbuffer - Win32 Release"

# Begin Custom Build
InputPath=.\cg_shimmer_dtestFP.cg
InputName=cg_shimmer_dtestFP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ELSEIF  "$(CFG)" == "simple_float_pbuffer - Win32 Debug"

# Begin Custom Build
InputPath=.\cg_shimmer_dtestFP.cg
InputName=cg_shimmer_dtestFP

"$(InputName).fp" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	"$(CG_BIN_PATH)\cgc.exe"   "$(InputPath)" -o "$(InputName).fp" -profile arbfp1

# End Custom Build

!ENDIF 

# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\Cg\lib\cgGL.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\Cg\lib\cg.lib
# End Source File
# End Target
# End Project
