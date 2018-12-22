// PennyMatchingDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PennyMatching.h"
#include "PennyMatchingDlg.h"

// include predictors
#include "RandomPredictor.h"
#include "SimplePredictor.h"
#include "StringMatchPredictor.h"
#include "ImprovedPredictor.h"
#include "SelfAdjustingPredictor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define HEAD 0
#define TAIL 1

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CString	m_BackgroundInfo;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	m_BackgroundInfo = _T("");
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Text(pDX, IDC_BACKGROUND_INFO, m_BackgroundInfo);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPennyMatchingDlg dialog

CPennyMatchingDlg::CPennyMatchingDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPennyMatchingDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPennyMatchingDlg)
	m_MachineScore = _T("");
	m_NumberOfPlays = _T("");
	m_SuccessRatio = _T("");
	m_YourScore = _T("");
	m_ThresholdRatio = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPennyMatchingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPennyMatchingDlg)
	DDX_Control(pDX, IDC_TAIL, m_Tail);
	DDX_Control(pDX, IDC_HEAD, m_Head);
	DDX_Control(pDX, IDC_TOGGLESTATS, m_ToggleStatistics);
	DDX_Control(pDX, IDC_PREDICTOR, m_Predictors);
	DDX_Text(pDX, IDC_MACHINE_SCORE, m_MachineScore);
	DDX_Text(pDX, IDC_PLAYS, m_NumberOfPlays);
	DDX_Text(pDX, IDC_RATIO, m_SuccessRatio);
	DDX_Text(pDX, IDC_YOUR_SCORE, m_YourScore);
	DDX_Text(pDX, IDC_THRESHOLDRATIO, m_ThresholdRatio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPennyMatchingDlg, CDialog)
	//{{AFX_MSG_MAP(CPennyMatchingDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_CREATE()
	ON_CBN_SELCHANGE(IDC_PREDICTOR, OnSelChangePredictor)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_HEAD, OnHead)
	ON_BN_CLICKED(IDC_TAIL, OnTail)
	ON_BN_CLICKED(IDC_TOGGLESTATS, OnToggleStats)
	ON_BN_CLICKED(IDC_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPennyMatchingDlg message handlers

BOOL CPennyMatchingDlg::OnInitDialog()
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
	
	// add random predictor
	CRandomPredictor * pRandomPredictor = new CRandomPredictor;
	pRandomPredictor->Setup(2, GetTickCount());
	RegisterPredictor("Random predictor (bad performance)", pRandomPredictor);

	// add simple string-matching predictor
	CSimplePredictor * pSimplePredictor = new CSimplePredictor;
	pSimplePredictor->Setup(1000, 5);
	RegisterPredictor("O(N^2) String matching predictor (good performance but slow)", pSimplePredictor);

	// add efficient string-matching predictor
	CStringMatchPredictor * pStringMatchPredictor = new CStringMatchPredictor;
	pStringMatchPredictor->Setup(1000, 2, 5);
	RegisterPredictor("O(N) String matching predictor (good performance and fast)", pStringMatchPredictor);

	// add improved string-matching predictor
	CImprovedPredictor * pImprovedPredictor = new CImprovedPredictor;
	pImprovedPredictor->Setup(1000, 2, 0.6f);
	RegisterPredictor("Improved O(N) String matching predictor (best performance and fast)", pImprovedPredictor);

	// add improved string-matching predictor
	CSelfAdjustingPredictor * pSelfAdjustingPredictor = new CSelfAdjustingPredictor;
	pSelfAdjustingPredictor->Setup(1000, 2, 0.6f);
	RegisterPredictor("Self adjusting O(N) String matching predictor", pSelfAdjustingPredictor);

	/*HICON hIcon;
	hIcon = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_HEAD));
	m_Head.SetIcon( hIcon );
	hIcon = ::LoadIcon(AfxGetResourceHandle(), MAKEINTRESOURCE(IDI_TAIL));
	m_Tail.SetIcon( hIcon );*/

	//m_Head.AutoLoad(IDB_HEAD, this); //LoadBitmaps(MAKEINTRESOURCE(IDB_HEAD));
	//m_Tail.AutoLoad(IDB_TAIL, this);

	m_hBitmapHead = ::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_HEAD)); 
	m_Head.SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (long)m_hBitmapHead);

	m_hBitmapTail = ::LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_TAIL)); 
	m_Tail.SendMessage(BM_SETIMAGE, IMAGE_BITMAP, (long)m_hBitmapTail);


/*
Specifies that the button displays a bitmap (BS_BITMAP) instead of text.Type: Bool. Default:
 False. 
The BS_BITMAP is a new button style in Windows 95.  After a button is created with this 
style, assign the bitmap to the button by sending a BM_SETIMAGE message to the button with
 the wParam as IMAGE_BITMAP and lParam as a handle to the bitmap. Windows displays the 
 specified bitmap on the button.  The attached bitmap should not be deleted until the button
 is destroyed. 
*/

	m_bExpanded = true;
	OnToggleStats();
	ResetGame();	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CPennyMatchingDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CPennyMatchingDlg::OnPaint() 
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
HCURSOR CPennyMatchingDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

int CPennyMatchingDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	return 0;
}

void CPennyMatchingDlg::OnSelChangePredictor() 
{
	ResetGame();
}

void CPennyMatchingDlg::RegisterPredictor(const CString &PredictorName, CPredictor *pPredictor)
{
	int nIndex = m_Predictors.AddString(PredictorName);
	m_Predictors.SetItemData(nIndex, (DWORD)pPredictor);
	m_Predictors.SetCurSel(nIndex);
}

void CPennyMatchingDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// release memory occupied by the predictors
	for (int i = 0; i<m_Predictors.GetCount(); i++) {
		CPredictor * pPredictor = (CPredictor *)m_Predictors.GetItemData(i);
		delete pPredictor;
	}
}

void CPennyMatchingDlg::ResetGame()
{
	int nIndex = m_Predictors.GetCurSel();
	m_pPredictor = (CPredictor *)m_Predictors.GetItemData(nIndex);

	m_nPlays = 0;
	m_nMachineWins = 0;
	m_nPlayerWins = 0;
	m_nTotalPredictions = 0;
	m_nSuccessfulPredictions = 0;

	UpdateDisplay();
}

void CPennyMatchingDlg::UpdateDisplay()
{
	m_MachineScore.Format("%d", m_nMachineWins);
	m_YourScore.Format("%d", m_nPlayerWins);

	if (m_nPlays > 0) {
		m_SuccessRatio.Format("%d%%", (100 * m_nMachineWins) / m_nPlays);
	} else {
		m_SuccessRatio = "N/A";
	}

	if (m_nTotalPredictions > 0) {
		m_ThresholdRatio.Format("%d%%", (100 * m_nSuccessfulPredictions) / m_nTotalPredictions);
	} else {
		m_ThresholdRatio = "N/A";
	}

	m_NumberOfPlays.Format("%d", m_nPlays);

	UpdateData(FALSE);
}

void CPennyMatchingDlg::OnHead() 
{
	ProcessSelection(HEAD);
}

void CPennyMatchingDlg::OnTail() 
{
	ProcessSelection(TAIL);
}

void CPennyMatchingDlg::ProcessSelection(int Selection) {
	int Prediction;

	if (m_pPredictor->GetPrediction(Prediction)) {
		m_nTotalPredictions++;
		if (Prediction==Selection) {
			m_nSuccessfulPredictions++;
		}
	}

	if (Prediction==Selection) {
		m_nMachineWins++;	
	} else {
		m_nPlayerWins++;
	}

	m_nPlays++;
	m_pPredictor->Update(Selection);
	UpdateDisplay();
}

void CPennyMatchingDlg::OnToggleStats() 
{
	CRect rcDialog;
	int nNewHeight;
	GetWindowRect( &rcDialog );

	if (m_bExpanded) {
		nNewHeight = rcDialog.Height() / 2;
		m_ToggleStatistics.SetWindowText("Show &statistics >>");

	} else {
		nNewHeight = rcDialog.Height() * 2;
		m_ToggleStatistics.SetWindowText("Hide &statistics <<");
	}

	SetWindowPos(NULL, 0, 0, rcDialog.Width(), nNewHeight, SWP_NOMOVE | SWP_NOZORDER );

	/*
	CWnd * pWndControl = GetWindow( GW_CHILD );
	while ( pWndControl != NULL) {
		CRect rcControl;
		pWndControl->GetWindowRect( &rcControl );
		pWndControl->EnableWindow( rcControl.top <= rcDialog.top + nNewHeight;
		pWndControl = pWndControl->GetWindow( GW_HWNDNEXT );
	}
	*/

	m_bExpanded = !m_bExpanded;
}

int CAboutDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here
	m_BackgroundInfo = "'At Bell Laboratories David Hagelbarger built a simple mind-reading machine, whose purpose\r\r\nwas to play the \"penny matching\" game. In this game, a player chooses head or tail, while\r\na mind-reading machine tries to predict and match his/her choice.\r\n\r\nHagelbargers simple 8-state machine was able to match the \"pennies\" of its human\r\nopponent 5218 times over a course of 9795 plays. Hagelbarger's machine recorded and \r\nanalyzed its human opponent's past choices, looking for patterns that would foretell the\r\nnext choice; since it is almost impossible for a human to avoid falling into such patterns,\r\nHagelbarger's machine could be successful in more than 50% of the time.'";
	
	return 0;
}

void CPennyMatchingDlg::OnAbout() 
{
	CAboutDlg dlg;
	dlg.DoModal();
}
