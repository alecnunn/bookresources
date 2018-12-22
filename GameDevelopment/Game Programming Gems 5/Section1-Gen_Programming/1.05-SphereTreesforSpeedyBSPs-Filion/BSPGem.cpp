//
// BSPGem.cpp
//

#include "stdafx.h"
#include "BSPGem.h"
#include "BSPGemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// The one and only CBSPGemApp object
CBSPGemApp theApp;

BEGIN_MESSAGE_MAP(CBSPGemApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CBSPGemApp::CBSPGemApp()
{
}

BOOL CBSPGemApp::InitInstance()
{
	CWinApp::InitInstance();

	CBSPGemDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();

	// Since the dialog has been closed, return FALSE so that we exit the application, rather than start the application's message pump.
	return FALSE;
}