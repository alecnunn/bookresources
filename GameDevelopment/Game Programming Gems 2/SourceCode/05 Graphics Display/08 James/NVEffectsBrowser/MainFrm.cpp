/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "NVEffectsBrowser.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndToolBar.CreateEx(this) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}
	/*if (!m_wndDlgBar.Create(this, IDR_MAINFRAME, 
		CBRS_ALIGN_TOP, AFX_IDW_DIALOGBAR))
	{
		TRACE0("Failed to create dialogbar\n");
		return -1;		// fail to create
	}*/

	if (!m_wndReBar.Create(this) ||
		!m_wndReBar.AddBar(&m_wndToolBar) /*||
		!m_wndReBar.AddBar(&m_wndDlgBar)*/)/*
	{
		TRACE0("Failed to create rebar\n");
		return -1;      // fail to create
	}*/

	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	// create splitter window
	if (!m_wndSplitter0.CreateStatic(this, 1, 3))
		return FALSE;

	// third splitter.  Contains program elements.
	if (!m_wndSplitter1.CreateStatic(&m_wndSplitter0, 2, 1, WS_CHILD | WS_VISIBLE, m_wndSplitter0.IdFromRowCol(0, 1)))
	{
		TRACE("Failed to create second splitter\n");
		return FALSE;
	}

	BOOL failed = false;
	failed = failed || !m_wndSplitter0.CreateView(0, 0, RUNTIME_CLASS(CShaderTree), CSize(100, 100), pContext);
	failed = failed || !m_wndSplitter1.CreateView(0, 0, RUNTIME_CLASS(CProgramFormView), CSize(100, 100), pContext);
	failed = failed || !m_wndSplitter1.CreateView(1, 0, RUNTIME_CLASS(CProgramFormView), CSize(100, 100), pContext);
	failed = failed || !m_wndSplitter0.CreateView(0, 2, RUNTIME_CLASS(CRenderView), CSize(100, 100), pContext);
	if (failed)
	{
		LPVOID lpMsgBuf;
		FormatMessage( 
			FORMAT_MESSAGE_ALLOCATE_BUFFER | 
			FORMAT_MESSAGE_FROM_SYSTEM | 
			FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL,
			GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf,
			0,
			NULL 
			);
		// Process any inserts in lpMsgBuf.
		// ...
		// Display the string.
		MessageBox( (LPCTSTR)lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION );
		// Free the buffer.
		LocalFree( lpMsgBuf );

		m_wndSplitter0.DestroyWindow();
		m_wndSplitter1.DestroyWindow();
		return FALSE;
	}

	m_wndSplitter0.SetColumnInfo(0, m_S0Column0, 1);
	m_wndSplitter0.SetColumnInfo(1, m_S0Column1, 1);
	m_wndSplitter1.SetRowInfo(0, m_S1Row0, 0);

	CProgramFormView* pProgramFormView = (CProgramFormView*)m_wndSplitter1.GetPane(0, 0);
	pProgramFormView->SetViewType(VIEWTYPE_VERTEX);
	pProgramFormView = (CProgramFormView*)m_wndSplitter1.GetPane(1, 0);
	pProgramFormView->SetViewType(VIEWTYPE_PIXEL);

	m_pRenderView = (CRenderView*) m_wndSplitter0.GetPane(0, 2);

	return TRUE;
}

#define DEFAULT_WNDX 900
#define DEFAULT_WNDY 700
#define DEFAULT_COL0 200
#define DEFAULT_COL1 300
#define DEFAULT_ROW0 250

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	m_wndX = theApp.GetProfileInt("Sizes", "FrameWindowX", DEFAULT_WNDX);
	m_wndY = theApp.GetProfileInt("Sizes", "FrameWindowY", DEFAULT_WNDY);
	CHECK_RANGE(m_wndX, 640, 1600);
	CHECK_RANGE(m_wndY, 480, 1200);

	m_S0Column0 = theApp.GetProfileInt("Sizes", "S0Column0", DEFAULT_COL0);
	m_S0Column1 = theApp.GetProfileInt("Sizes", "S0Column1", DEFAULT_COL1);
	m_S1Row0 = theApp.GetProfileInt("Sizes", "S1Row0", DEFAULT_ROW0);
	CHECK_RANGE(m_S0Column1, 1, m_wndX);
	CHECK_RANGE(m_S0Column0, 1, m_wndX);
	CHECK_RANGE(m_S1Row0, 1, m_wndY);

	if ((m_wndX - m_S0Column1 - m_S0Column0) < 100)
	{
		m_S0Column0 = DEFAULT_COL0;
		m_S0Column1 = DEFAULT_COL1;
		m_S1Row0 = DEFAULT_ROW0;
		m_wndX = DEFAULT_WNDX;
		m_wndY = DEFAULT_WNDY;
	}

	if ((m_wndY - m_S1Row0) < 100)
	{
		m_S0Column0 = DEFAULT_COL0;
		m_S0Column1 = DEFAULT_COL1;
		m_S1Row0 = DEFAULT_ROW0;
		m_wndX = DEFAULT_WNDX;
		m_wndY = DEFAULT_WNDY;
	}

	cs.cx = m_wndX;
	cs.cy = m_wndY;

	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;

	return TRUE;
}

void CMainFrame::OnClose() 
{
	CRect rcWnd;
	int Temp;
	GetWindowRect(rcWnd);

	DWORD dwStyle = GetStyle();
	if (dwStyle & WS_MAXIMIZE)
		theApp.WriteProfileInt("Sizes", "FrameWindowMaximized", 1);
	else
		theApp.WriteProfileInt("Sizes", "FrameWindowMaximized", 0);
	
	theApp.WriteProfileInt("Sizes", "FrameWindowX", rcWnd.Width());
	theApp.WriteProfileInt("Sizes", "FrameWindowY", rcWnd.Height());

	m_wndSplitter1.GetRowInfo(0, m_S1Row0, Temp);
	theApp.WriteProfileInt("Sizes", "S1Row0", m_S1Row0);

	m_wndSplitter0.GetColumnInfo(0, m_S0Column0, Temp);
	theApp.WriteProfileInt("Sizes", "S0Column0", m_S0Column0);
	m_wndSplitter0.GetColumnInfo(1, m_S0Column1, Temp);
	theApp.WriteProfileInt("Sizes", "S0Column1", m_S0Column1);
		
	
	CFrameWnd::OnClose();
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	// Route keyboard presses to the display window.
	if ((pMsg->message == WM_KEYDOWN) || (pMsg->message == WM_KEYUP))
	{
		if (m_pRenderView)
		{
			CRenderWnd *pWnd = m_pRenderView->GetCurWnd();
			if (pWnd) {
				pMsg->hwnd = pWnd->GetSafeHwnd();
				::TranslateMessage(pMsg);
				::DispatchMessage(pMsg);
				return true;
			}
		}
	}

	return CFrameWnd::PreTranslateMessage(pMsg);
}
