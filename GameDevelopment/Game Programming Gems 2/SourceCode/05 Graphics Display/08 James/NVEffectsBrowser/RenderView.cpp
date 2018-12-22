/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// RenderView.cpp : implementation file
//

#include "StdAfx.h"
#include "NVEffectsBrowser.h"
#include "RenderView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenderView

IMPLEMENT_DYNCREATE(CRenderView, CView)

CRenderView::CRenderView()
  : m_fResizing(false),
    m_eRenderMode(NoRenderMode),
	m_pCurWnd(NULL),
    m_pRenderD3DWnd(NULL),
	m_pRenderOGLWnd(NULL)
{
	SetMsg("Initializing...");
}

CRenderView::~CRenderView()
{
	if (m_pRenderD3DWnd)
	{
		m_pRenderD3DWnd->Release();
		delete m_pRenderD3DWnd;
	}

	if (m_pRenderOGLWnd)
	{
		m_pRenderOGLWnd->Release();
		delete m_pRenderOGLWnd;
	}
}


BEGIN_MESSAGE_MAP(CRenderView, CView)
	//{{AFX_MSG_MAP(CRenderView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SIZING()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenderView attributes

CNVEffectsDoc* CRenderView::GetDocument()
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CNVEffectsDoc)));
	return (CNVEffectsDoc*) m_pDocument;
}

/////////////////////////////////////////////////////////////////////////////
// CRenderView drawing

void CRenderView::OnDraw(CDC* pDC)
{
	CNVEffectsDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code here

	// Fill this window with grey
	CRect r;
	COLORREF c = 0x00AAAAAA;
	GetClientRect(r);
	pDC->FillSolidRect(r, c);

	// Do not redraw text while resizing
	if (!m_fResizing) {

		// Write the current message text...
		pDC->DrawText(m_strMsg, r, DT_CENTER | DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	}

	// Repaint the window, it has been disrupted
	if (m_pCurWnd)
		m_pCurWnd->Refresh(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CRenderView diagnostics

#ifdef _DEBUG
void CRenderView::AssertValid() const
{
	CView::AssertValid();
}

void CRenderView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRenderView methods

HRESULT CRenderView::SetRenderMode(RenderMode mode)
{
	if (m_eRenderMode == mode)
		return S_OK;

	CRenderWnd *pOldWnd = m_pCurWnd;
	CRenderWnd *pNewWnd;
	CString str;

	switch (mode) {
	case D3DRenderMode:
		pNewWnd = m_pRenderD3DWnd;
		str = "Switching to Direct3D rendering mode...";
		break;
	case OGLRenderMode:
		pNewWnd = m_pRenderOGLWnd;
		str = "Switching to OpenGL rendering mode...";
		break;
	default:
		ASSERT(0 && "Unknown render mode!");
		break;
	}

	// Hide the old rendering window
	if (pOldWnd)
		pOldWnd->ShowWindow(SW_HIDE);

	// Update the window message
	SetMsg(str, true);

	// Release the old rendering window resources
	if (pOldWnd)
		pOldWnd->Release();

	// Initialize the new rendering window
	if (pNewWnd)
		pNewWnd->Init();

	m_pCurWnd = pNewWnd;
	m_eRenderMode = mode;

	return S_OK;
}

void CRenderView::UpdateDevice(EBEffect *pEffect)
{
	// Hide the current window
	m_pCurWnd->ShowWindow(SW_HIDE);

	// Update the window message
	SetMsg("Resetting the window...", true);

	// Update the rendering window device
	m_pCurWnd->UpdateDevice();

	// Reinstall the current effect, if there is one
	if (pEffect)
		GetDocument()->SetCurrentEffect(pEffect);
}

void CRenderView::SetMsg(const CString &str, BOOL fForceRedraw)
{
	m_strMsg = str;
	if (GetSafeHwnd() && IsWindowVisible() &&
		(!m_pCurWnd || fForceRedraw))
		OnDraw(GetDC());
}

/////////////////////////////////////////////////////////////////////////////
// CRenderView message handlers

BOOL CRenderView::PreCreateWindow(CREATESTRUCT& cs) 
{
	// TODO: Add your specialized code here and/or call the base class

	cs.dwExStyle |= WS_EX_CLIENTEDGE;

	return CView::PreCreateWindow(cs);
}

int CRenderView::OnCreate(LPCREATESTRUCT lpcs) 
{
	if (CView::OnCreate(lpcs) == -1)
		return -1;
	
	// TODO: Add your specialized creation code here

	// Setup to build the windows
	DWORD dwStyle = 0x0;
	CCreateContext *pContext = (CCreateContext*) lpcs->lpCreateParams;

	// Create the D3D rendering window
	m_pRenderD3DWnd = (CRenderD3DWnd*) RUNTIME_CLASS(CRenderD3DWnd)->CreateObject();
	if ((m_pRenderD3DWnd == NULL) ||
	    (!m_pRenderD3DWnd->Create(NULL, NULL, dwStyle,
				       CRect(CPoint(0,0), CSize(100, 100)),
				       this, D3DRenderMode, pContext))) {

	  // Unable to create
	  TRACE("Unable to create D3D rendering window!");
	  return -1;
	}

	// Create the OGL rendering window
	m_pRenderOGLWnd = (CRenderOGLWnd*) RUNTIME_CLASS(CRenderOGLWnd)->CreateObject();
	if ((m_pRenderOGLWnd == NULL) ||
	    (!m_pRenderOGLWnd->Create(NULL, NULL, dwStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
                       CRect(CPoint(0,0), CSize(100, 100)),
				       this, OGLRenderMode, pContext))) {

	  // Unable to create
	  TRACE("Unable to create OpenGL rendering window!");
	  return -1;
	}

	return 0;
}

void CRenderView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here

	// Resize the child rendering windows
	CRect rect(CPoint(0,0), CSize(cx,cy));
	m_pRenderD3DWnd->MoveWindow(rect);
	m_pRenderOGLWnd->MoveWindow(rect);

	// Show the appropriate window
	if (m_pCurWnd && !m_pCurWnd->IsWindowVisible())
		m_pCurWnd->ShowWindow(SW_SHOW);

	m_fResizing = false;
}

void CRenderView::OnSizing(UINT fwSide, LPRECT pRect) 
{
	CView::OnSizing(fwSide, pRect);
	
	// TODO: Add your message handler code here

	// Hide the current window while resizing
	if (m_pCurWnd && m_pCurWnd->IsWindowVisible())
		m_pCurWnd->ShowWindow(SW_HIDE);

	// First call?
	if (!m_fResizing) {
		m_fResizing = true;
		OnDraw(GetDC());
	}
}
