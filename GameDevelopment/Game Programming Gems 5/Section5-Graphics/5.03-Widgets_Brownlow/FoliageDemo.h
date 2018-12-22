// FoliageDemo.h : main header file for the FOLIAGEDEMO application
//

#if !defined(AFX_FOLIAGEDEMO_H__1B37277A_0A8D_45E1_8183_1D38C993CD99__INCLUDED_)
#define AFX_FOLIAGEDEMO_H__1B37277A_0A8D_45E1_8183_1D38C993CD99__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "types.h"
#include "worldview.h"

/////////////////////////////////////////////////////////////////////////////
// CFoliageDemoApp:
// See FoliageDemo.cpp for the implementation of this class
//

class CFoliageDemoApp : public CWinApp
{
public:
	CFoliageDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFoliageDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CFoliageDemoApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FOLIAGEDEMO_H__1B37277A_0A8D_45E1_8183_1D38C993CD99__INCLUDED_)
