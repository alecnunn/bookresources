// SphinxMMOSPlayerMainFrame.cpp : Implementierung der Klasse CSphinxMMOSPlayerMainFrame
//

#include "stdafx.h"
#include "SphinxMMOSPlayer.h"

#include "SphinxMMOSPlayerMainFrame.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerMainFrame

IMPLEMENT_DYNCREATE(CSphinxMMOSPlayerMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CSphinxMMOSPlayerMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CSphinxMMOSPlayerMainFrame)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // Statusleistenanzeige
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerMainFrame Konstruktion/Zerstörung

CSphinxMMOSPlayerMainFrame::CSphinxMMOSPlayerMainFrame()
{
}

CSphinxMMOSPlayerMainFrame::~CSphinxMMOSPlayerMainFrame()
{
}

int CSphinxMMOSPlayerMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Symbolleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Statusleiste konnte nicht erstellt werden\n");
		return -1;      // Fehler bei Erstellung
	}

	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CSphinxMMOSPlayerMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerMainFrame Diagnose

#ifdef _DEBUG
void CSphinxMMOSPlayerMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CSphinxMMOSPlayerMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSphinxMMOSPlayerMainFrame Nachrichten-Handler

