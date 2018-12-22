/* Copyright (C) Ian Lewis, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Ian Lewis, 2001"
 */
// LowLevelAPI.h : main header file for the LOWLEVELAPI application
//

#if !defined(AFX_LOWLEVELAPI_H__20FB44E0_C71F_4DCC_96EF_C551304361B0__INCLUDED_)
#define AFX_LOWLEVELAPI_H__20FB44E0_C71F_4DCC_96EF_C551304361B0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CLowLevelAPIApp:
// See LowLevelAPI.cpp for the implementation of this class
//

class CLowLevelAPIApp : public CWinApp
{
public:
	CLowLevelAPIApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLowLevelAPIApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CLowLevelAPIApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOWLEVELAPI_H__20FB44E0_C71F_4DCC_96EF_C551304361B0__INCLUDED_)
