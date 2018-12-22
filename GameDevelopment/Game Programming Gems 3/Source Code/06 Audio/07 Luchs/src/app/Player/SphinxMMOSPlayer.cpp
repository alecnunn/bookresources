// SphinxMMOSPlayer.cpp : Legt das Klassenverhalten für die Anwendung fest.
//

#include "stdafx.h"
#include "SphinxMMOSPlayer.h"

#include "SphinxMMOSPlayerMainFrame.h"
#include "SphinxMMOSPlayerDoc.h"
#include "SphinxMMOSPlayerView.h"
#include "resource.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerApp

BEGIN_MESSAGE_MAP(CSphinxMMOSPlayerApp, CWinApp)
	//{{AFX_MSG_MAP(CSphinxMMOSPlayerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
	// Dateibasierte Standard-Dokumentbefehle
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerApp Konstruktion

CSphinxMMOSPlayerApp::CSphinxMMOSPlayerApp()
{
}

/////////////////////////////////////////////////////////////////////////////

CSphinxMMOSPlayerApp theApp;
atomos::IGASystem* g_pGASystem = NULL;

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerApp Initialisierung

BOOL CSphinxMMOSPlayerApp::InitInstance()
{
	InitEngine();


#ifdef _AFXDLL
	Enable3dControls();			
#else
	Enable3dControlsStatic();	
#endif



	SetRegistryKey(_T("Visiomedia"));

	LoadStdProfileSettings(8);  

	// Dokumentvorlagen registrieren

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CSphinxMMOSPlayerDoc),
		RUNTIME_CLASS(CSphinxMMOSPlayerMainFrame),       // Haupt-SDI-Rahmenfenster
		RUNTIME_CLASS(CSphinxMMOSPlayerView));
	AddDocTemplate(pDocTemplate);

	// DDE-Execute-Open aktivieren
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Verteilung der in der Befehlszeile angegebenen Befehle
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();

	// Öffnen mit Drag & Drop aktivieren
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

void CSphinxMMOSPlayerApp::InitEngine()
{
	HINSTANCE hInstance = AfxGetInstanceHandle();

	// create the global engine object and load the audio plugin
	InitializeAtomos();

	if(g_pEngine != NULL) {
		g_pEngine->CreateApplication(CID_Application, hInstance, _T("SphinxMMOSPlayer"));
		if(g_pGASystem != NULL)
			g_pGASystem->ReadConfiguration(_T("sphinxmmos.cfg"));
	}
	else {

		AfxMessageBox("Couldn't init engine.\r\n"
			"atombase and atomgaud must be in your start directory");
	}

}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg-Dialog für Info über Anwendung

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialogdaten
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_strVersion;
	//}}AFX_DATA

	// Überladungen für virtuelle Funktionen, die vom Anwendungs-Assistenten erzeugt wurden
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV-Unterstützung
	//}}AFX_VIRTUAL

// Implementierung
protected:
	//{{AFX_MSG(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_strVersion = _T("Version 0603");
	//}}AFX_DATA_INIT

}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_VERSION, m_strVersion);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// Keine Nachrichten-Handler
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Anwendungsbefehl zum Ausführen des Dialogfelds
void CSphinxMMOSPlayerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerApp-Nachrichtenbehandlungsroutinen

int CSphinxMMOSPlayerApp::ExitInstance() 
{
	if(g_pEngine != NULL)
		g_pEngine->Terminate();
	
	
	return CWinApp::ExitInstance();
}
