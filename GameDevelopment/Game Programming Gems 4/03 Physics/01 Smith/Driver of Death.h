// Driver of Death.h : main header file for the DRIVER OF DEATH application
//

#if !defined(AFX_DRIVEROFDEATH_H__48A23102_9A5E_4EB3_8814_5ACC482E2DB2__INCLUDED_)
#define AFX_DRIVEROFDEATH_H__48A23102_9A5E_4EB3_8814_5ACC482E2DB2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CDriverofDeathApp:
// See Driver of Death.cpp for the implementation of this class
//

class CDriverofDeathApp : public CWinApp
{
public:
	CDriverofDeathApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDriverofDeathApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CDriverofDeathApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DRIVEROFDEATH_H__48A23102_9A5E_4EB3_8814_5ACC482E2DB2__INCLUDED_)
