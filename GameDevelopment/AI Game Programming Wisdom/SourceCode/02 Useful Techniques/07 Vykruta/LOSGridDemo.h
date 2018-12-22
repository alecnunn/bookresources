// LOSGridDemo.h : main header file for the LOSGRIDDEMO application
//

#if !defined(AFX_LOSGRIDDEMO_H__6F71D254_D83E_11D5_BA44_000102368FA3__INCLUDED_)
#define AFX_LOSGRIDDEMO_H__6F71D254_D83E_11D5_BA44_000102368FA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLOSGridDemoApp:
// See LOSGridDemo.cpp for the implementation of this class
//

class CLOSGridDemoApp : public CWinApp
{
public:
	CLOSGridDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLOSGridDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CLOSGridDemoApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOSGRIDDEMO_H__6F71D254_D83E_11D5_BA44_000102368FA3__INCLUDED_)
