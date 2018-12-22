// RTTIProps.h : main header file for the RTTIPROPS application
//

#if !defined(AFX_RTTIPROPS_H__6365659A_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_)
#define AFX_RTTIPROPS_H__6365659A_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsApp:
// See RTTIProps.cpp for the implementation of this class
//

class CRTTIPropsApp : public CWinApp
{
public:
	CRTTIPropsApp();

  bool m_boActivated;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTTIPropsApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CRTTIPropsApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern CRTTIPropsApp theApp;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTTIPROPS_H__6365659A_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_)
