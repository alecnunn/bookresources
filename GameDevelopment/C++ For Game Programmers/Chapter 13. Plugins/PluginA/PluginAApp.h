// PluginA.h : main header file for the PluginA DLL
//

#if !defined(AFX_PluginA_H__58DF3362_968E_4C4F_89EB_F8A60CDA6CA9__INCLUDED_)
#define AFX_PluginA_H__58DF3362_968E_4C4F_89EB_F8A60CDA6CA9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPluginAApp
// See PluginA.cpp for the implementation of this class
//

class CPluginAApp : public CWinApp
{
public:
	CPluginAApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPluginAApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPluginAApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PluginA_H__58DF3362_968E_4C4F_89EB_F8A60CDA6CA9__INCLUDED_)
