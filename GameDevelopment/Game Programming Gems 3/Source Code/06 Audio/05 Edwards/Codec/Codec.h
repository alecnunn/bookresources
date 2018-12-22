// Codec.h : main header file for the CODEC application
//

#if !defined(AFX_CODEC_H__F28DB126_7898_11D2_9FAB_0060082C7A8C__INCLUDED_)
#define AFX_CODEC_H__F28DB126_7898_11D2_9FAB_0060082C7A8C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCodecApp:
// See Codec.cpp for the implementation of this class
//

class CCodecApp : public CWinApp
{
public:
	CCodecApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCodecApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCodecApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEC_H__F28DB126_7898_11D2_9FAB_0060082C7A8C__INCLUDED_)
