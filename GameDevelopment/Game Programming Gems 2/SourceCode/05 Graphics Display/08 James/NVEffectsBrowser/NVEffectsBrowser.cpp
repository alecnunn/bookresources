/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// NVEffectsBrowser.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NVEffectsBrowser.h"

#include "MainFrm.h"
#include "Doc.h"
#include "RenderView.h"
#include "StatLink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

nv_objects::NVDebug dbg(3, "out.txt");

extern CRenderView* g_pAppFormView;

/////////////////////////////////////////////////////////////////////////////
// CNVEffects

BEGIN_MESSAGE_MAP(CNVEffects, CWinApp)
	//{{AFX_MSG_MAP(CNVEffects)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNVEffects construction

CNVEffects::CNVEffects()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNVEffects object

CNVEffects theApp;


/////////////////////////////////////////////////////////////////////////////
// CNVEffects initialization

BOOL CNVEffects::InitInstance()
{
#ifdef _DEBUG
	_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	_CrtSetReportMode ( _CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("NVIDIA Corporation"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register document templates

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CNVEffectsDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CRenderView));
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	m_DevType = D3DDEVTYPE_HAL;

	int DeviceType = theApp.GetProfileInt("Settings", "Device", 0);
	m_bForceSoftwareVP = (theApp.GetProfileInt("Settings", "ForceSoftwareVP", 0) > 0) ? 1 : 0;
	switch(DeviceType)
	{
		case 0:
		default:
			m_DevType = D3DDEVTYPE_HAL;
			break;
		case 1:
			m_DevType = D3DDEVTYPE_REF;
			break;
		case 2:
			m_DevType = D3DDEVTYPE_SW;
			break;
	}

	m_bExpandAll = (theApp.GetProfileInt("Settings", "ExpandAll", 1) > 0) ? 1 : 0;

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(m_pMainWnd);
	if (pFrameWnd)
	{
		CNVEffectsDoc* pDoc = static_cast<CNVEffectsDoc*>(pFrameWnd->GetActiveDocument());
		if (pDoc)
		{
			bool bStart = pDoc->Start();
			if (!bStart)
				return false;
		}
	}

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	CStaticLink m_devwebLink;		// static text
	CStaticLink m_devwebLinkImage;	// Image
	CStaticLink m_emailLink;		// static text
	
	//////////////////
	// Initialize dialog: subclass static controls
	//
	virtual BOOL OnInitDialog() {
		m_devwebLink.SubclassDlgItem(IDC_WEBLINK, this);
		m_devwebLinkImage.SubclassDlgItem(IDC_WEBLINK2, this);
		m_emailLink.SubclassDlgItem(IDC_EMAILLINK, this);
		
		m_devwebLinkImage.m_link = _T("http://www.nvidia.com/developer.nsf");
		m_emailLink.m_link = _T("mailto:devrelfeedback@nvidia.com&subject=NVEffectsBrowser");

		return CDialog::OnInitDialog();
	}


// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CNVEffects::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CNVEffects message handlers


BOOL CNVEffects::OnIdle(LONG lCount) 
{
	// Do not render if the app is minimized
    if( m_pMainWnd->IsIconic() )
        return FALSE;

	if (CWinApp::OnIdle(lCount))
		return true;

    // Update and render a frame
	CMainFrame* pFrameWnd = static_cast<CMainFrame*>(GetMainWnd());
	if (pFrameWnd)
	{
		CNVEffectsDoc* pDoc = static_cast<CNVEffectsDoc*>(pFrameWnd->GetActiveDocument());
		if (pDoc)
		{
			pDoc->OnIdle();
		}
	}	

	return true;
	
}


BOOL CNVEffects::InitApplication() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	return CWinApp::InitApplication();
}

int CNVEffects::ExitInstance() 
{
	switch(m_DevType)
	{
		default:
		case D3DDEVTYPE_HAL:
			theApp.WriteProfileInt("Settings", "Device", 0);
			break;

		case D3DDEVTYPE_REF:
			theApp.WriteProfileInt("Settings", "Device", 1);
			break;

		case D3DDEVTYPE_SW:
			theApp.WriteProfileInt("Settings", "Device", 2);
			break;
	}
	
	theApp.WriteProfileInt("Settings", "ForceSoftwareVP", (int)m_bForceSoftwareVP);
	theApp.WriteProfileInt("Settings", "ExpandAll", (int)m_bExpandAll);
	
#if CHECK_LEAKS_BEFORE_EXIT
	// This function is called when the app exits, the only real correct time for dumps
	// We do it here to get a chance to look at the memory, but be warned it will contain
	// memory from other libraries.
	_CrtDumpMemoryLeaks();
#endif
	return CWinApp::ExitInstance();
}

BOOL CNVEffects::PreTranslateMessage(MSG* pMsg) 
{
	if ((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
	{
		CMainFrame* pFrameWnd = static_cast<CMainFrame*>(GetMainWnd());
		if (pFrameWnd)
			if (pFrameWnd->PreTranslateMessage(pMsg))
				return true;
	}
	
	return CWinApp::PreTranslateMessage(pMsg);
}
