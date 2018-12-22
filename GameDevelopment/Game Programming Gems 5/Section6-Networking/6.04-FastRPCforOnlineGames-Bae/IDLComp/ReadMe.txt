========================================================================
    CONSOLE APPLICATION : IDLComp Project Overview
========================================================================

IDLComp is a compiler which takes an IDL file and generates proxy and files
for our own RPC implementations.

IDLComp consists of two parts: front end and back end. The front end exists
in IDL.g,IncludeGeneratedFiles.cpp,IncludeGeneratedFiles.h,Specification.cpp and
Specification.h. The back end exists Backend.cpp and Backend.h.

IDLComp.vcproj
    This is the main project file for VC++ projects generated using an Application Wizard. 
    It contains information about the version of Visual C++ that generated the file, and 
    information about the platforms, configurations, and project features selected with the
    Application Wizard.

IDLComp.cpp
    This is the main application source file.

/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named IDLComp.pch and a precompiled types file named StdAfx.obj.

/////////////////////////////////////////////////////////////////////////////
Other notes:

AppWizard uses "TODO:" comments to indicate parts of the source code you
should add to or customize.

/////////////////////////////////////////////////////////////////////////////
