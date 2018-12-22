/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "csg_bsp.h"
#include "MainFrm.h"
#include "z_ed2Doc.h"
#include "z_ed2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


//---------------------------------------------------------------------------------------
// Main application message handlers map to handler functions
BEGIN_MESSAGE_MAP(CZ_ed2App, CWinApp)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
// Constructor for the Application Instance (should be singleton but is MFC...)
CZ_ed2App::CZ_ed2App()
{
}

//---------------------------------------------------------------------------------------
CZ_ed2App theApp;                               // the main instance and the only one

//---------------------------------------------------------------------------------------
BOOL CZ_ed2App::InitInstance()
{
    Dbg_ResolveCurDir();                        // If runs from debug changes the path
    AfxEnableControlContainer();                // to ..\bin\ (if have not been set)
#ifdef _AFXDLL
    Enable3dControls();			
#else
    Enable3dControlsStatic();	
#endif
    SetRegistryKey(_T("csg using bsps"));           
    LoadStdProfileSettings();  
    CSingleDocTemplate* pDocTemplate;
    pDocTemplate = new CSingleDocTemplate(
                       IDR_MAINFRAME,
                       RUNTIME_CLASS(CZ_ed2Doc),
                       RUNTIME_CLASS(CMainFrame),       
                       RUNTIME_CLASS(CZ_ed2View));
    AddDocTemplate(pDocTemplate);
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;
    m_pMainWnd->ShowWindow(SW_SHOW);
    m_pMainWnd->UpdateWindow();
    return TRUE;
}

//---------------------------------------------------------------------------------------
class CAboutDlg : public CDialog
{
public:
    CAboutDlg();
    enum { IDD = IDD_ABOUTBOX };
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    DECLARE_MESSAGE_MAP()
};

//---------------------------------------------------------------------------------------
CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

//---------------------------------------------------------------------------------------
void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
}
//---------------------------------------------------------------------------------------
BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

//---------------------------------------------------------------------------------------
void CZ_ed2App::OnAppAbout()
{
    CAboutDlg aboutDlg;
    aboutDlg.DoModal();
}

//---------------------------------------------------------------------------------------
void    CZ_ed2App::Dbg_ResolveCurDir()
{
    char sPath[_MAX_PATH];
    _getcwd(sPath,_MAX_PATH);
    if(!strstr(sPath,"/bin")){
        if(0!=chdir("../bin/")){
            chdir("bin/");
        }
    }
}

//---------------------------------------------------------------------------------------
int CZ_ed2App::ExitInstance()
{
    return CWinApp::ExitInstance();
}
