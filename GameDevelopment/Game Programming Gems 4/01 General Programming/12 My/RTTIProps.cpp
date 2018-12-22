// RTTIProps.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "RTTIProps.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "RTTIPropsDoc.h"
#include "RTTIPropsView.h"

#include "CustomMsgs.h"
#include "Engine.h"
#include "RendererDX8.h"

#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsApp

BEGIN_MESSAGE_MAP(CRTTIPropsApp, CWinApp)
	//{{AFX_MSG_MAP(CRTTIPropsApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
	// Standard file based document commands
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsApp construction

CRTTIPropsApp::CRTTIPropsApp()
  {
  m_boActivated = true;
  }

/////////////////////////////////////////////////////////////////////////////
// The one and only CRTTIPropsApp object

CRTTIPropsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsApp initialization

BOOL CRTTIPropsApp::InitInstance()
  {
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("GPG4"));

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
		IDR_RTTIPRTYPE,
		RUNTIME_CLASS(CRTTIPropsDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		NULL); //RUNTIME_CLASS(CRTTIPropsView));
	AddDocTemplate(pDocTemplate);

  //

  char szBuffer[_MAX_PATH];                                 // get the current working directory
  _getcwd(szBuffer,_MAX_PATH);
  _mkdir("levels");

  CRTTIPropsDoc::SetLevelPath  (CString(szBuffer)+"\\levels");
  CRTTIPropsDoc::SetTexturePath(CString(szBuffer)+"\\data\\textures");
  CRTTIPropsDoc::SetMeshPath   (CString(szBuffer)+"\\data\\meshes");

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
  cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;  // disable creation of empty document at start-up
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pMainFrame->UpdateWindow();

  //
  u32 u32Flags = GPG4::IRenderer::WINDOWED | GPG4::IRenderer::ZBUFFER;
  if(!GPG4::CEngine::GetEngine()->CreateRenderer(m_pMainWnd->m_hWnd,u32Flags,1024,768))
    {
    MessageBox(NULL,"renderer initialization failed","ERROR",MB_ICONEXCLAMATION|MB_OK);
    return FALSE;
    }

	return TRUE;
  }

//

int CRTTIPropsApp::ExitInstance() 
  {
  GPG4::CEngine::GetEngine()->ReleaseRenderer();
	return CWinApp::ExitInstance();
  }

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

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
	afx_msg void OnCodeproject();
	afx_msg void OnFairyengine();
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
	ON_BN_CLICKED(IDC_CODEPROJECT, OnCodeproject)
	ON_BN_CLICKED(IDC_FAIRYENGINE, OnFairyengine)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CAboutDlg::OnCodeproject() 
{
  ShellExecute(NULL,"open","http://www.codeproject.com/miscctrl/gridctrl.asp","","",SW_SHOW);
}

void CAboutDlg::OnFairyengine() 
{
  ShellExecute(NULL,"open","http://www.fairyengine.com","","",SW_SHOW);
}

// App command to run the dialog
void CRTTIPropsApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CRTTIPropsApp message handlers

void CRTTIPropsApp::OnFileNew() 
  {
  CWinApp::OnFileNew();
  }

//

void CRTTIPropsApp::OnFileOpen() 
  {
	CFileDialog dlgOpen(TRUE,"xml",NULL,OFN_NOCHANGEDIR,"RTTIProps level (*.xml)|*.xml||");
  dlgOpen.m_ofn.lpstrInitialDir = CRTTIPropsDoc::GetLevelPath();
  if(dlgOpen.DoModal() != IDOK) return;

  OpenDocumentFile(dlgOpen.GetPathName());
  }

//
//

BOOL CRTTIPropsApp::OnIdle(LONG lCount) 
  {
	if(CWinApp::OnIdle(lCount)) return TRUE;
  if(!m_boActivated) return FALSE;

  // find active document - The MFC Answer Book, 2.18
  CWnd* pWnd = AfxGetMainWnd();
  if(pWnd)
    {
    CFrameWnd* pMainFrame   = static_cast<CFrameWnd*>(pWnd);
    CFrameWnd* pActiveFrame = pMainFrame->GetActiveFrame();

    if(pActiveFrame)
      {
      CDocument* pDoc = pActiveFrame->GetActiveDocument();
      if(pDoc)
        {
        pDoc->UpdateAllViews(NULL,IDLE);
        return TRUE;                                        // give me more processing time
        }
      }
    }

  return FALSE;
  }
