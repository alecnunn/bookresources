// crackme_N2_GIU.h : main header file for the CRACKME_N2_GIU application
//

#if !defined(AFX_CRACKME_N2_GIU_H__6A659E5C_C750_47B3_AF4F_BAAB272BCD7D__INCLUDED_)
#define AFX_CRACKME_N2_GIU_H__6A659E5C_C750_47B3_AF4F_BAAB272BCD7D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCrackme_N2_GIUApp:
// See crackme_N2_GIU.cpp for the implementation of this class
//

class CCrackme_N2_GIUApp : public CWinApp
{
public:
	CCrackme_N2_GIUApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCrackme_N2_GIUApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCrackme_N2_GIUApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CRACKME_N2_GIU_H__6A659E5C_C750_47B3_AF4F_BAAB272BCD7D__INCLUDED_)
