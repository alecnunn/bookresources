/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
// InterpDemo.h : main header file for the INTERPDEMO application
//

#if !defined(AFX_INTERPDEMO_H__04D78084_FEA4_11D3_9A3F_004F49077001__INCLUDED_)
#define AFX_INTERPDEMO_H__04D78084_FEA4_11D3_9A3F_004F49077001__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CInterpDemoApp:
// See InterpDemo.cpp for the implementation of this class
//

class CInterpDemoApp : public CWinApp
{
public:
	CInterpDemoApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterpDemoApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CInterpDemoApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERPDEMO_H__04D78084_FEA4_11D3_9A3F_004F49077001__INCLUDED_)
