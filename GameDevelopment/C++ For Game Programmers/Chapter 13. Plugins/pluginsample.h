// pluginsample.h : main header file for the PLUGINSAMPLE application
//

#if !defined(AFX_PLUGINSAMPLE_H__F8DCBB3E_1C29_4F37_B749_A4DDB6819DDD__INCLUDED_)
#define AFX_PLUGINSAMPLE_H__F8DCBB3E_1C29_4F37_B749_A4DDB6819DDD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// PluginSample:
// See pluginsample.cpp for the implementation of this class
//

class PluginSample : public CWinApp
{
public:
	PluginSample();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(PluginSample)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(PluginSample)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLUGINSAMPLE_H__F8DCBB3E_1C29_4F37_B749_A4DDB6819DDD__INCLUDED_)
