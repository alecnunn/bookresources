// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "LOSGridDemo.h"
#include "ChildView.h"
#include "TransformMat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Custom code
CPaintDC* g_dc = NULL;

const int size = ELEMWID*10;
const int origin1 = 50, origin2 = origin1 + 50 + size;

Layer g_Layer( 0, 0, size/ELEMWID,size/ELEMWID, 50.f);

RECT rcGrid1;
RECT rcGrid2;

BOOL bHighlited = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CChildView

CChildView::CChildView()
{
}

CChildView::~CChildView()
{
}


BEGIN_MESSAGE_MAP(CChildView,CWnd )
	//{{AFX_MSG_MAP(CChildView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN( )
	ON_WM_RBUTTONDOWN( )
	ON_WM_MOUSEMOVE ( )
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_CROSS), HBRUSH(COLOR_WINDOW+1), NULL);

	SetRect(&rcGrid1, origin1, origin1, origin1 + size, origin1 + size);
	SetRect(&rcGrid2, origin2, origin1, origin2 + size, origin1 + size);

	return TRUE;
}

void DrawGrid(RECT &rcGrid)
{
	int x, y;
	x = rcGrid.left;
	y = rcGrid.top;

	while (x <= rcGrid.right)
	{
		g_dc->MoveTo(x, rcGrid.top);
		g_dc->LineTo(x, rcGrid.bottom);
		
		g_dc->MoveTo(rcGrid.left, y);
		g_dc->LineTo(rcGrid.right, y);
		x += 20;
		y += 20;
	}

	int iLeft = rcGrid.left + (rcGrid.right - rcGrid.left)/2;
	int iTop = rcGrid.top + (rcGrid.bottom - rcGrid.top)/2;
	
	RECT rcCenter;
	SetRect(&rcCenter, iLeft-3, iTop-3, iLeft+3, iTop+3);
	g_dc->Rectangle(&rcCenter);
}

void DrawLine(VECTOR2D vOrig, VECTOR2D vLine, int iColorType)
{
    HPEN hpen, hpenold;

	 COLORREF rgb = (COLORREF)(0xff << (iColorType << 3));
    // Create a pen.
    hpen = CreatePen(PS_SOLID, 2, rgb);

    // Select the new pen and brush, and then draw.
    hpenold = (HPEN)g_dc->SelectObject( hpen);
    
	int iLeft = rcGrid1.left + (rcGrid1.right - rcGrid1.left)/2;
	int iTop = rcGrid1.top + (rcGrid1.bottom - rcGrid1.top)/2;
	g_dc->MoveTo(iLeft + vOrig.x,     iTop + vOrig.y);
	g_dc->RoundRect(iLeft + vOrig.x, iTop + vOrig.y, iLeft + vOrig.x + 5, iTop + vOrig.y + 5, 5, 5);
	g_dc->MoveTo(iLeft + vOrig.x,     iTop + vOrig.y);
	g_dc->LineTo(iLeft + vOrig.x + vLine.x, iTop + vOrig.y + vLine.y);
	
	g_dc->SelectObject(hpenold);
   DeleteObject(hpen);
}

void DrawLine2(VECTOR2D vOrig, VECTOR2D vLine, int iColorType)
{
    HPEN hpen, hpenold;

	 COLORREF rgb = (COLORREF)(0xff << (iColorType << 3));
    // Create a pen.
    hpen = CreatePen(PS_SOLID, 2, rgb);

    // Select the new pen and brush, and then draw.
    hpenold = (HPEN)g_dc->SelectObject( hpen);
    
	int iLeft = rcGrid2.left + (rcGrid2.right - rcGrid2.left)/2;
	int iTop = rcGrid2.top + (rcGrid2.bottom - rcGrid2.top)/2;
	g_dc->MoveTo(iLeft + vOrig.x,     iTop + vOrig.y);
	g_dc->RoundRect(iLeft + vOrig.x, iTop + vOrig.y, iLeft + vOrig.x + 5, iTop + vOrig.y + 5, 5, 5);	
	g_dc->MoveTo(iLeft + vOrig.x,     iTop + vOrig.y);
	g_dc->LineTo(iLeft + vOrig.x + vLine.x, iTop + vOrig.y + vLine.y);
	
	g_dc->SelectObject(hpenold);
   DeleteObject(hpen);
}

VECTOR2D vDir(50,-50);
VECTOR2D vOrig (20,10);
void CChildView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	g_dc = &dc;

	// select system font
	HGDIOBJ hfnt; 
	hfnt = GetStockObject(ANSI_FIXED_FONT); 
	SelectObject(dc.m_hDC, hfnt);

	if (bHighlited)
	{
		RECT rcHL = rcGrid2;
		rcHL.right += 2;
		rcHL.bottom += 2;
		g_dc->Rectangle(&rcHL);
	}

	dc.TextOut( rcGrid2.left, rcGrid2.bottom + 5, "*Right/Left click to set ray");
	dc.TextOut( rcGrid2.left + 20, rcGrid2.top - 20, "Real Grid Space");

	dc.TextOut( rcGrid1.left + 20, rcGrid1.top - 20, "Virtual Grid Space");
	dc.TextOut( rcGrid2.left + 20, rcGrid2.top - 20, "Real Grid Space");
	
	DrawGrid(rcGrid1);
	DrawGrid(rcGrid2);

	VECTOR3D vFrom, vTo, vImpact;
	vFrom.Set(vOrig.x, 0, vOrig.y);
	vFrom += g_Layer.GetOrigin();
	
	vTo.Set(vDir.x, 0, vDir.y);
	vTo += vFrom;
	vTo += g_Layer.GetOrigin();
	
	g_Layer.DoesElemCollideRay(vFrom, vTo, &vImpact);
};


inline VECTOR2D rectCenter(const RECT& rc)
{
	VECTOR2D vRet;
	vRet.x = rc.left + (rc.right - rc.left)/2;
	vRet.y = rc.top  + (rc.bottom - rc.top)/2;
	return(vRet);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (PtInRect(&rcGrid2, point) )
	{
		VECTOR2D vTemp = vOrig + vDir;
		vOrig = VECTOR2D(point.x,point.y) - rectCenter(rcGrid2);
		vDir = vTemp - vOrig;
	}

	Invalidate();
	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	if (PtInRect(&rcGrid2, point) )
		vDir = VECTOR2D(point.x,point.y) - rectCenter(rcGrid2) - vOrig;

	Invalidate();
	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnMouseMove( UINT nFlags, CPoint point )
{
	if (bHighlited != PtInRect(&rcGrid2, point) )
	{
		bHighlited = !bHighlited;
		Invalidate();
//		SetCursor(LoadCursor(NULL, bHighlited ? IDC_CROSS : IDC_ARROW));
	}
}
