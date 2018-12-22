// SphinxMMOSPlayer.h : Haupt-Header-Datei für die Anwendung DEMO
//

#if !defined(AFX_DEMO_H__E40830EF_6292_42BE_BDD0_E0DF6EF459D6__INCLUDED_)
#define AFX_DEMO_H__E40830EF_6292_42BE_BDD0_E0DF6EF459D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // Hauptsymbole

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerApp:
// Siehe SphinxMMOSPlayer.cpp für die Implementierung dieser Klasse
//

class CSphinxMMOSPlayerApp : public CWinApp
{
public:
	CSphinxMMOSPlayerApp();


	void InitEngine();

// Überladungen
	// Vom Klassenassistenten generierte Überladungen virtueller Funktionen
	//{{AFX_VIRTUAL(CSphinxMMOSPlayerApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementierung
	//{{AFX_MSG(CSphinxMMOSPlayerApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern atomos::IGASystem* g_pGASystem;

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ fügt unmittelbar vor der vorhergehenden Zeile zusätzliche Deklarationen ein.

#endif // !defined(AFX_DEMO_H__E40830EF_6292_42BE_BDD0_E0DF6EF459D6__INCLUDED_)
