/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// linkerdialog.h : main header file for the LINKERDIALOG DLL
//

#if !defined(AFX_LINKERDIALOG_H__F5B33E2A_4264_4992_ABE3_68F28A90928D__INCLUDED_)
#define AFX_LINKERDIALOG_H__F5B33E2A_4264_4992_ABE3_68F28A90928D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#include "TLDialog.h"
#include "linkercommon.h"
#include <assert.h>

/////////////////////////////////////////////////////////////////////////////
// CLinkerdialogApp
// See linkerdialog.cpp for the implementation of this class
//

class CLinkerDialog : public CWinApp
{
public:
	CLinkerDialog();
	BOOL ShowDialog(bool bShow);
	BOOL Initialise(HWND hWnd);
	BOOL Free();
	BOOL GetSettings(DialogSettings* pSettings);
	BOOL SetSettings(DialogSettings* pSettings);
	BOOL UpdateEnables();

private:
	CWnd	m_ParentWnd;
	CTLDialog m_TLOptions;
	bool m_bShow;
	DialogSettings m_Settings;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLinkerDialog)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CLinkerDialog)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LINKERDIALOG_H__F5B33E2A_4264_4992_ABE3_68F28A90928D__INCLUDED_)
