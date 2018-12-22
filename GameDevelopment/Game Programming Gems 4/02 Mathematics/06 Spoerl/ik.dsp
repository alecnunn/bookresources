# Microsoft Developer Studio Project File - Name="ik" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** NICHT BEARBEITEN **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=ik - Win32 Debug
!MESSAGE Dies ist kein gültiges Makefile. Zum Erstellen dieses Projekts mit NMAKE
!MESSAGE verwenden Sie den Befehl "Makefile exportieren" und führen Sie den Befehl
!MESSAGE 
!MESSAGE NMAKE /f "ik.mak".
!MESSAGE 
!MESSAGE Sie können beim Ausführen von NMAKE eine Konfiguration angeben
!MESSAGE durch Definieren des Makros CFG in der Befehlszeile. Zum Beispiel:
!MESSAGE 
!MESSAGE NMAKE /f "ik.mak" CFG="ik - Win32 Debug"
!MESSAGE 
!MESSAGE Für die Konfiguration stehen zur Auswahl:
!MESSAGE 
!MESSAGE "ik - Win32 Release" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE "ik - Win32 Debug" (basierend auf  "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "Perforce Project"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ik - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "_tmp/release"
# PROP Intermediate_Dir "_tmp/release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "NDEBUG"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"_bin/release/ik.exe"

!ELSEIF  "$(CFG)" == "ik - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "_tmp/debug"
# PROP Intermediate_Dir "_tmp/debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /FD /GZ /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE RSC /l 0x407 /d "_DEBUG"
# ADD RSC /l 0x407 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"_bin/debug/ik.exe" /pdbtype:sept

!ENDIF 

# Begin Target

# Name "ik - Win32 Release"
# Name "ik - Win32 Debug"
# Begin Group "Glut"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\glut\glut.h
# End Source File
# Begin Source File

SOURCE=.\glut\glut32.lib
# End Source File
# End Group
# Begin Group "GLFont"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\glfont\GLFont.cpp
# End Source File
# Begin Source File

SOURCE=.\glfont\GLFont.h
# End Source File
# End Group
# Begin Group "Application"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\app\CApplication.cpp
# End Source File
# Begin Source File

SOURCE=.\app\CApplication.h
# End Source File
# Begin Source File

SOURCE=.\app\CTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\app\CTarget.h
# End Source File
# End Group
# Begin Group "Math"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\math\Basics.cpp
# End Source File
# Begin Source File

SOURCE=.\math\Basics.h
# End Source File
# Begin Source File

SOURCE=.\math\CAxisAngle.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CAxisAngle.h
# End Source File
# Begin Source File

SOURCE=.\math\CEulerAngles.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CEulerAngles.h
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix3.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix3.h
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix4.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CMatrix4.h
# End Source File
# Begin Source File

SOURCE=.\math\CQuaternion.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CQuaternion.h
# End Source File
# Begin Source File

SOURCE=.\math\CSphericalRotation.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CSphericalRotation.h
# End Source File
# Begin Source File

SOURCE=.\math\CVector3.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CVector3.h
# End Source File
# Begin Source File

SOURCE=.\math\CVector4.cpp
# End Source File
# Begin Source File

SOURCE=.\math\CVector4.h
# End Source File
# End Group
# Begin Group "Inverse Kinematics"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\ik\CIKSolver.cpp
# End Source File
# Begin Source File

SOURCE=.\ik\CIKSolver.h
# End Source File
# Begin Source File

SOURCE=.\ik\CIKSolverCyclicCoordinateDescent.cpp
# End Source File
# Begin Source File

SOURCE=.\ik\CIKSolverCyclicCoordinateDescent.h
# End Source File
# Begin Source File

SOURCE=.\ik\CIKSolverJacobianTranspose.cpp
# End Source File
# Begin Source File

SOURCE=.\ik\CIKSolverJacobianTranspose.h
# End Source File
# Begin Source File

SOURCE=.\ik\CNode.cpp
# End Source File
# Begin Source File

SOURCE=.\ik\CNode.h
# End Source File
# Begin Source File

SOURCE=.\ik\IIKSolver.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\ik.cpp
# SUBTRACT CPP /YX /Yc /Yu
# End Source File
# End Target
# End Project
