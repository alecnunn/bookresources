// Codec.cpp
//
// MFC CWinApp class

#include "stdafx.h"
#include "Codec.h"
#include "CodecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CCodecApp, CWinApp)
	//{{AFX_MSG_MAP(CCodecApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

CCodecApp::CCodecApp()
{
}

CCodecApp theApp;

BOOL CCodecApp::InitInstance()
{
#ifdef _AFXDLL
	Enable3dControls();
#else
	Enable3dControlsStatic();
#endif

	CCodecDlg dlg;
	m_pMainWnd = &dlg;
	dlg.DoModal();

	SoundManager::Exit();

	return FALSE;
}
