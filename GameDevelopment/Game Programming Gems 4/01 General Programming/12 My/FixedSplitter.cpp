// FixedSplitter.cpp : implementation file
//

#include "stdafx.h"
#include "RTTIProps.h"
#include "FixedSplitter.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFixedSplitter

IMPLEMENT_DYNCREATE(CFixedSplitter, CSplitterWnd)

CFixedSplitter::CFixedSplitter()
{
}

CFixedSplitter::~CFixedSplitter()
{
}

BEGIN_MESSAGE_MAP(CFixedSplitter, CSplitterWnd)
	//{{AFX_MSG_MAP(CFixedSplitter)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFixedSplitter message handlers

void CFixedSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CWnd::OnLButtonDown(nFlags, point);
	//CSplitterWnd::OnLButtonDown(nFlags, point);
}

void CFixedSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	CWnd::OnMouseMove(nFlags, point);
	//CSplitterWnd::OnLButtonDown(nFlags, point);
}

BOOL CFixedSplitter::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
//	return CSplitterWnd::OnSetCursor(pWnd, nHitTest, message);
}
