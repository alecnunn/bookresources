// ReflectionGem.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ReflectionGem.h"
#include "ReflectionGemDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CReflectionGemApp theApp;

BEGIN_MESSAGE_MAP(CReflectionGemApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

CReflectionGemApp::CReflectionGemApp()
{
}

BOOL CReflectionGemApp::InitInstance()
{
	CWinApp::InitInstance();


	CReflectionGemDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
