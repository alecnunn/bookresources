========================================================================
       A Generic Finite State Machine in C++ Sample Implementation
========================================================================


The Microsoft Foundation Classes AppWizard was used to create this GameGems 
application for you.  This application not only demonstrates the Generic FSM 
but also the basics of using the Microsoft Foundation classes as a starting 
point for writing an application.

This file contains a summary of what you will find in each of the files that
make up the Generic FSM in C++ application.

GameGems.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

GameGems.h
    This is the main header file for the application.  It includes other
    project specific headers (including Resource.h) and declares the
    CGameGemsApp application class.

GameGems.cpp
    This is the main application source file that contains the application
    class CGameGemsApp.

FSMclass.cpp
	This is the implementation of the FSMclass class, which manages the generic 
	FSM in C++.

FSMclass.h
	The definition of the FSMclass.

FSMstate.cpp
	This is the implementation of the FSMstate class, which provides the services
	for a specific state within the generic FSM.

FSMstate.h
	The definition of the FSMstate.

TestFSMDlg.cpp
	This is the implementation of a simple dialog that can be used to demonstrate
	the generic FSM in use.  It provides a means to input to the FSM and displays
	the current and output states that transistion.

TestFSMDlg.h
	The definition of the TestFSMDlg.

GameGems.rc
    This is a listing of all of the Microsoft Windows resources that the
    program uses.  It includes the icons, bitmaps, and cursors that are stored
    in the RES subdirectory.  This file can be directly edited in Microsoft
	Visual C++.

GameGems.clw
    This file contains information used by ClassWizard to edit existing
    classes or add new classes.  ClassWizard also uses this file to store
    information needed to create and edit message maps and dialog data
    maps and to create prototype member functions.

res\GameGems.ico
    This is an icon file, which is used as the application's icon.  This
    icon is included by the main resource file GameGems.rc.

res\GameGems.rc2
    This file contains resources that are not edited by Microsoft 
	Visual C++.  You should place all resources not editable by
	the resource editor in this file.


NOTE: In order to build this application without errors, be sure to create
the ..\res sub-directory and place the appropriate files within it.

/////////////////////////////////////////////////////////////////////////////

For the main frame window:

MainFrm.h, MainFrm.cpp
    These files contain the frame class CMainFrame, which is derived from
    CFrameWnd and controls all SDI frame features.

/////////////////////////////////////////////////////////////////////////////



/////////////////////////////////////////////////////////////////////////////
Other standard files:

StdAfx.h, StdAfx.cpp
    These files are used to build a precompiled header (PCH) file
    named GameGems.pch and a precompiled types file named StdAfx.obj.

Resource.h
    This is the standard header file, which defines new resource IDs.
    Microsoft Visual C++ reads and updates this file.

/////////////////////////////////////////////////////////////////////////////
