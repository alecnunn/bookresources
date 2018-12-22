// crackme_N2_GIUDlg.cpp : implementation file
//

#include "stdafx.h"
#include "crackme_N2_GIU.h"
#include "crackme_N2_GIUDlg.h"
#include "..\crackme.N2.main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
_pm_press();
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

/////////////////////////////////////////////////////////////////////////////
// CCrackme_N2_GIUDlg dialog

CCrackme_N2_GIUDlg::CCrackme_N2_GIUDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCrackme_N2_GIUDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCrackme_N2_GIUDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCrackme_N2_GIUDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCrackme_N2_GIUDlg)
	DDX_Control(pDX, IDC_BUTTON1, m_chk);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCrackme_N2_GIUDlg, CDialog)
	//{{AFX_MSG_MAP(CCrackme_N2_GIUDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrackme_N2_GIUDlg message handlers

BOOL CCrackme_N2_GIUDlg::OnInitDialog()
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
	_pm_press();
	m_chk.SetFocus();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCrackme_N2_GIUDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CCrackme_N2_GIUDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCrackme_N2_GIUDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


#define MAX_BUF_SIZE	0x1000

_pm_press()
{
	AfxGetMainWnd( )->SetDlgItemText(IDC_WAIT1, sPRESS);
	return 0;
}


/*

_pm_print_ICQ()
{
	AfxGetMainWnd( )->SetDlgItemText(IDC_WAIT1, sICQ);
	return 0;
}
*/

_pm_clear()
{
	AfxGetMainWnd( )->SetDlgItemText(IDC_WAIT1, "завершено");
	return 0;
	_ICQ_display_progress(0,100,100);

}

void CCrackme_N2_GIUDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	int a;
	m_ok.EnableWindow(0);
	
	a = protect_main(0, 0,_MY_POINT_IN_SEC_, _MY_KEY_BOUND_);
	//_beginthread((void (__cdecl*)(void*))protect_main, 0, 0);
	
	switch(a)
	{
		case _NOT_READY:	AfxMessageBox(sNOT_READY_ERR);
							break;

		case _NOT_SUPPORT:	AfxMessageBox(sNOT_SUPPORT);
							break;

		case _HELLO_HACKER:		AfxMessageBox(sHACKER); break;
		case _HELLO_LEGAL_USER:	AfxMessageBox(sLEGAL);	break;

	}
	_pm_clear();
	m_ok.EnableWindow(1);	
}
