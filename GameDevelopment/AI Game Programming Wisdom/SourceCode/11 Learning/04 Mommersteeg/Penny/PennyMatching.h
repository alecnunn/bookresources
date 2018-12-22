// PennyMatching.h : main header file for the PENNYMATCHING application
//

#if !defined(AFX_PENNYMATCHING_H__16AEA785_A0B4_11D5_B5E8_0010A7018204__INCLUDED_)
#define AFX_PENNYMATCHING_H__16AEA785_A0B4_11D5_B5E8_0010A7018204__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPennyMatchingApp:
// See PennyMatching.cpp for the implementation of this class
//

class CPennyMatchingApp : public CWinApp
{
public:
	CPennyMatchingApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPennyMatchingApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPennyMatchingApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PENNYMATCHING_H__16AEA785_A0B4_11D5_B5E8_0010A7018204__INCLUDED_)
