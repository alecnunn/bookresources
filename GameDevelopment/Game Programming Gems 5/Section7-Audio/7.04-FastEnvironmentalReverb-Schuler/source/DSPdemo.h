// DSPdemo.h : main header file for the DSPDEMO application
//

#if !defined(AFX_DSPDEMO_H__E1D87F0A_426F_4338_BFC6_189F57578A70__INCLUDED_)
#define AFX_DSPDEMO_H__E1D87F0A_426F_4338_BFC6_189F57578A70__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDSPdemoApp:
// See DSPdemo.cpp for the implementation of this class
//

class CDSPdemoApp : public CWinApp
{
public:
	CDSPdemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDSPdemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDSPdemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DSPDEMO_H__E1D87F0A_426F_4338_BFC6_189F57578A70__INCLUDED_)
