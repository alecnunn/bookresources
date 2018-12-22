/**********************************************************************

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2003
 **********************************************************************/
// DstarViewer.h : main header file for the DstarViewer application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CDstarViewerApp:
// See DstarViewer.cpp for the implementation of this class
//

class CDstarViewerApp : public CWinApp
{
public:
	CDstarViewerApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	afx_msg void OnSetBase();
	DECLARE_MESSAGE_MAP()
};

extern CDstarViewerApp theApp;