// ReflectionGem.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "PropertySystem.h"

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// CReflectionGemApp
//
// Main MFC application class. Note that a property system object is instantiated here. Freeing the property
// system will deallocate properties as well.
//
// Nothing exciting happening here, this is standard MFC setup.
//
///////////////////////////////////////////////////////////////////////////////////////////////////

class CReflectionGemApp : public CWinApp
{

public:

	//----------------------------------------------------------------------------------------------
	CReflectionGemApp();

	//----------------------------------------------------------------------------------------------
	// Standard MFC instance initialization.
	virtual BOOL InitInstance();

private :

	CPropertySystem	m_PropertySystem;	// Our property system instance

	DECLARE_MESSAGE_MAP()

};

extern CReflectionGemApp theApp;