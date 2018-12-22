// FoliageDemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "FoliageDemo.h"
#include "FoliageDemoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
// CFoliageDemoDlg dialog

CFoliageDemoDlg::CFoliageDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFoliageDemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFoliageDemoDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFoliageDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFoliageDemoDlg)
	DDX_Control(pDX, IDC_WIDGETDISTANCE, m_drawDistance);
	DDX_Control(pDX, IDC_FREQUENCY, m_frequency);
	DDX_Control(pDX, IDC_WORLDVIEW, m_worldView);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CFoliageDemoDlg, CDialog)
	//{{AFX_MSG_MAP(CFoliageDemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_FREQUENCY, OnCustomdrawFrequency)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_WIDGETDISTANCE, OnCustomdrawWidgetdistance)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFoliageDemoDlg message handlers

BOOL CFoliageDemoDlg::OnInitDialog()
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

	m_frequency.SetRange(0,100);
	m_frequency.SetPos(50);

	m_drawDistance.SetRange(0,100);
	m_drawDistance.SetPos(50);
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CFoliageDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFoliageDemoDlg::OnPaint() 
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
HCURSOR CFoliageDemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CFoliageDemoDlg::OnCustomdrawFrequency(NMHDR* pNMHDR, LRESULT* pResult) 
{
	u32		uValue = m_frequency.GetPos();
	float	f;
	char	text[256];

	// log scale...
	f = powf(10,(uValue/100.0f))/10.0f;
	f = (f-0.1f)*10.0f/9.0f;

	uValue = (u32)Lerp(1000,100000,f);
	m_worldView.CreateWidgets(uValue);

	sprintf(text,"%2d",uValue);
	GetDlgItem(IDC_NUMWIDGETSTEXT)->SetWindowText(text);
	
	*pResult = 0;
}

void CFoliageDemoDlg::OnCustomdrawWidgetdistance(NMHDR* pNMHDR, LRESULT* pResult) 
{
	u32		uValue = m_drawDistance.GetPos();
	float	f;
	char	text[256];

	f = (uValue/100.0f);
	f = Lerp(50.0f,600.0f,f);
	m_worldView.SetDrawDistance(f);

	sprintf(text,"%3.1f",f);
	GetDlgItem(IDC_DRAWDISTANCETEXT)->SetWindowText(text);
	
	*pResult = 0;
}
