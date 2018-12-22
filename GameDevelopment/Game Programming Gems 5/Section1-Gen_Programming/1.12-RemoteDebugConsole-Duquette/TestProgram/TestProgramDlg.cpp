// TestProgramDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestProgram.h"
#include "TestProgramDlg.h"
#include ".\testprogramdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CTestProgramDlg dialog



CTestProgramDlg::CTestProgramDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestProgramDlg::IDD, pParent)
    , m_Edit_PageName(_T(""))
    , m_Edit_LogText(_T(""))
    , m_Edit_ValueName(_T(""))
    , m_Edit_Value(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTestProgramDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_PAGENAME, m_Edit_PageName);
    DDX_Text(pDX, IDC_EDIT_LOGTEXT, m_Edit_LogText);
    DDX_Text(pDX, IDC_EDIT_VALUENAME, m_Edit_ValueName);
    DDX_Text(pDX, IDC_EDIT_VALUE, m_Edit_Value);
    DDX_Control(pDX, IDC_COMBO_PAGENAME, m_Combo_PageName);
}

BEGIN_MESSAGE_MAP(CTestProgramDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BTN_STARTLOG, OnBnClickedStartlog)
    ON_BN_CLICKED(IDC_BTN_STOPLOG, OnBnClickedStoplog)
    ON_BN_CLICKED(IDC_BTN_PAUSE, OnBnClickedPause)
    ON_BN_CLICKED(IDC_BTN_ADDPAGE, OnBnClickedAddpage)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_BTN_ADDLOG, OnBnClickedBtnAddlog)
    ON_BN_CLICKED(IDC_BTN_ADDVALUE, OnBnClickedBtnAddvalue)
END_MESSAGE_MAP()


// CTestProgramDlg message handlers

BOOL CTestProgramDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    m_uiTimer = SetTimer( 1, 100, NULL );

    
    CheckRadioButton( IDC_RADIO_LIST, IDC_RADIO_VALUE, IDC_RADIO_LIST );

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTestProgramDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestProgramDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestProgramDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestProgramDlg::OnTimer( UINT_PTR nIDEvent )
{
    m_RemoteLogger.Tick();
}
void CTestProgramDlg::OnBnClickedStartlog()
{
    m_RemoteLogger.StartLog();
}

void CTestProgramDlg::OnBnClickedStoplog()
{
    m_RemoteLogger.StopLog();
    m_Combo_PageName.Clear();
}

void CTestProgramDlg::OnBnClickedPause()
{
    m_RemoteLogger.PauseLog();
}

void CTestProgramDlg::OnBnClickedAddpage()
{
    UpdateData();
    if( 1 > m_Edit_PageName.GetLength()  )
    {
        MessageBox( "Need a PageName", "Add Page", MB_ICONINFORMATION );
        return;
    }

    int iCheckedRadioID = GetCheckedRadioButton( IDC_RADIO_LIST, IDC_RADIO_VALUE );
    if( IDC_RADIO_LIST == iCheckedRadioID )
    {
        m_RemoteLogger.AddPage( RemoteLoggingGlobal::ePT_LOGLIST, (LPCTSTR) m_Edit_PageName );
    }
    else
    {
        m_RemoteLogger.AddPage( RemoteLoggingGlobal::ePT_VALUELIST, (LPCTSTR) m_Edit_PageName );
    }

    m_Combo_PageName.AddString( m_Edit_PageName );

    m_Edit_PageName = "";
    UpdateData( false );
}

void CTestProgramDlg::OnBnClickedBtnAddlog()
{
    UpdateData();

    if( 1 > m_Edit_LogText.GetLength() )
    {
        MessageBox( "Nothing to send ...", "Add to log", MB_ICONERROR );
        return;
    }

    if( 0 >= m_Combo_PageName.GetCount() )
    {
        MessageBox( "No page to send to ...", "Add to log", MB_ICONERROR );
        return;
    }

    if( RemoteLoggingGlobal::eDMS_LOGLISTPAGEDATA_STRING < (m_Edit_LogText.GetLength() + 1) )
    {
        MessageBox( "The maximum allowed string length to send is 255 characters", "Add to log", MB_ICONERROR );
        return;
    }

    CString strPageName;
    m_Combo_PageName.GetLBText( m_Combo_PageName.GetCurSel(), strPageName );

    m_RemoteLogger.AddLogValue( (LPCTSTR) strPageName, (LPCTSTR) m_Edit_LogText );

    m_Edit_LogText = "";
    UpdateData( false );

    // Put the focus back on the LogText field
    GetDlgItem( IDC_EDIT_LOGTEXT )->SetFocus();
}

void CTestProgramDlg::OnBnClickedBtnAddvalue()
{
    UpdateData();

    if( 1 > m_Edit_ValueName.GetLength() )
    {
        MessageBox( "Nothing to send ...", "Add Variable Value", MB_ICONERROR );
        return;
    }

    if( 1 > m_Edit_ValueName.GetLength() || 
        1 > m_Edit_Value.GetLength() )
    {
        MessageBox( "Nothing to send ...", "Add Variable Value", MB_ICONERROR );
        return;
    }

    if( 0 >= m_Combo_PageName.GetCount() )
    {
        MessageBox( "No page to send to ...", "Add Variable Value", MB_ICONERROR );
        return;
    }

    if( RemoteLoggingGlobal::eDMS_VALUEPAGEDATA_NAME < (m_Edit_ValueName.GetLength() + 1) )
    {
        MessageBox( "The maximum allowed Variable Name length to send is 64 characters", "Add Variable Value", MB_ICONERROR );
        return;
    }

    if( RemoteLoggingGlobal::eDMS_VALUEPAGEDATA_VALUE < (m_Edit_Value.GetLength() + 1) )
    {
        MessageBox( "The maximum allowed Variable Value length to send is 255 characters", "Add Variable Value", MB_ICONERROR );
        return;
    }

    CString strPageName;
    m_Combo_PageName.GetLBText( m_Combo_PageName.GetCurSel(), strPageName );

    m_RemoteLogger.AddVariableValue( (LPCTSTR) strPageName, (LPCTSTR) m_Edit_ValueName, (LPCTSTR) m_Edit_Value );

    m_Edit_Value = "";
    UpdateData( false );

    // Put the focus back on the Value field
    GetDlgItem( IDC_EDIT_VALUE )->SetFocus();
}

void CTestProgramDlg::OnCancel()
{
    m_RemoteLogger.StopLog();

    CDialog::OnCancel();
}
