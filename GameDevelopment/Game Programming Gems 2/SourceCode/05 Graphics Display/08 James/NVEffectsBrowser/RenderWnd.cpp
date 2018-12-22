/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// RenderWnd.cpp : implementation file
//

#include "stdafx.h"
#include "nveffectsbrowser.h"
#include "RenderWnd.h"

using namespace nv_objects;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd

IMPLEMENT_DYNAMIC(CRenderWnd, CWnd)

CRenderWnd::CRenderWnd()
{
}

CRenderWnd::~CRenderWnd()
{
	// XXX - why does this blow up?
	//HRESULT hr = Release();
	//ASSERT(SUCCEEDED(hr));
}


BEGIN_MESSAGE_MAP(CRenderWnd, CWnd)
	//{{AFX_MSG_MAP(CRenderWnd)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRenderWnd attributes

CNVEffectsDoc* CRenderWnd::GetDocument()
{
	ASSERT(GetParent()->IsKindOf(RUNTIME_CLASS(CRenderView)));
	CRenderView *pRenderView = (CRenderView*) GetParent();

	return pRenderView->GetDocument();
}

CRenderView* CRenderWnd::GetRenderView()
{
	ASSERT(GetParent()->IsKindOf(RUNTIME_CLASS(CRenderView)));
	CRenderView *pRenderView = (CRenderView*) GetParent();

	return pRenderView;
}

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd message handlers

void CRenderWnd::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	// Forward the resize event to the current effect, if there is one
	CNVEffectsDoc* pDoc = GetDocument();
	if (pDoc->GetCurrentEffect())
		pDoc->GetCurrentEffect()->Resize(GetSafeHwnd(), cx, cy, true);
	
}
void CRenderWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	pDoc->MouseMove(GetSafeHwnd(), point.x, point.y);
}

void CRenderWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	SetCapture();
	pDoc->MouseButton(GetSafeHwnd(), MOUSE_LEFTBUTTON, true, point.x, point.y);
}

void CRenderWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	ReleaseCapture();
	pDoc->MouseButton(GetSafeHwnd(), MOUSE_LEFTBUTTON, false, point.x, point.y);
}

void CRenderWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	ClientToScreen(&point);
	pDoc->DoTestPopup(point.x, point.y);
}

void CRenderWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CNVEffectsDoc* pDoc = GetDocument();
}

void CRenderWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	pDoc->Keyboard(nChar, nFlags, true);
}

void CRenderWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	pDoc->Keyboard(nChar, nFlags, false);
}


void CRenderWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	pDoc->Keyboard(nChar, nFlags, true);
}

void CRenderWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CNVEffectsDoc* pDoc = GetDocument();
	pDoc->Keyboard(nChar, nFlags, false);
}
