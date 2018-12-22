// aseView.cpp : implementation of the CAseView class
//

#include "stdafx.h"
#include "ase.h"

#include "aseDoc.h"
#include "aseView.h"

#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ci(x,y) x*ASE_BOARDY+y

/////////////////////////////////////////////////////////////////////////////
// CAseView

IMPLEMENT_DYNCREATE(CAseView, CView)

BEGIN_MESSAGE_MAP(CAseView, CView)
	//{{AFX_MSG_MAP(CAseView)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAseView construction/destruction

static COLORREF crBrushes [] = {
	RGB(255,255,255),
	RGB(192,192,192),
	RGB(128,128,128),
	RGB(000,000,000),
	RGB(255,000,000),
	RGB(000,255,000),
};

CAseView::CAseView()
{
	m_bDragging = false;
	m_uBrushType = 0;
}

CAseView::~CAseView()
{
}

BOOL CAseView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAseView drawing

void CAseView::OnDraw(CDC* pDC)
{
	CAseDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CRect rect;
	GetClientRect(rect);

	CDC memdc;
	CBitmap bmp, *oldbmp;
	CPen *oldpen, pen(PS_SOLID, 1, RGB(255,0,255));

	memdc.CreateCompatibleDC(GetDC());
	bmp.CreateDiscardableBitmap(pDC, rect.Width(), rect.Height());
	oldbmp = memdc.SelectObject(&bmp);
	oldpen = memdc.SelectObject(&pen);
	
	char *board = pDoc->GetBoard();
	
	memdc.FillSolidRect(rect, RGB(255,255,255));

	int i;
	int dx = rect.Width();
	int dy = rect.Height();

	// Cycle through the board and draw any squares

	for (i=0; i<ASE_BOARDX; i++) {
		for (int j=0; j<ASE_BOARDY; j++) {
			if (board[ci(i,j)] != 0) {
				DrawRectangle(i, j, &memdc, crBrushes[board[ci(i,j)]]);
			}
		}
	}

	// Draw start, end and debug points
	if (m_cStart.x != -1) DrawRectangle(m_cStart.x, m_cStart.y, &memdc, crBrushes[4]);
	if (m_cEnd.x != -1)   DrawRectangle(m_cEnd.x, m_cEnd.y, &memdc, crBrushes[5]);

	// Draw the grid...

	for (i=0; i<dx; i+=8) {
		for (int j=0;j<dy;j+=8) {
			memdc.SetPixel(i, j, RGB(192, 192, 192));
		}
	}

	// Draw route if necessary
	if (pDoc->DrawRoute()) DrawRoute(&memdc);

	if (m_cHilight.x != -1) {
		DrawRectangle(m_cHilight.x, m_cHilight.y, &memdc, RGB(0,0,255));
		
		CPoint centre;
		centre.x = m_cHilight.x*ASE_GRIDSIZE+(ASE_GRIDSIZE/2);
		centre.y = m_cHilight.y*ASE_GRIDSIZE+(ASE_GRIDSIZE/2);

		memdc.MoveTo(centre);

		int numchildren = m_cHilightNode->numchildren;
		_asNode *temp;
	
		for (int i=0; i<numchildren; i++) {
			temp = m_cHilightNode->children[i];
			memdc.LineTo(temp->x * ASE_GRIDSIZE + (ASE_GRIDSIZE/2),
				        temp->y * ASE_GRIDSIZE + (ASE_GRIDSIZE/2));
			memdc.MoveTo(centre);
		}
	}

	CPoint bp = pDoc->GetBreakpoint();
	if (bp.x != -1) DrawCircle(bp.x, bp.y, &memdc, RGB(128,0,0));

	pDC->BitBlt(0,0,rect.Width(), rect.Height(), &memdc, 0, 0, SRCCOPY);

	memdc.SelectObject(oldpen);
	memdc.SelectObject(oldbmp);
}

void CAseView::DrawRectangle(int x, int y, CDC *pDC, COLORREF cr)
{
	CBrush br(RGB(0,0,0));
	CRect rect(x*ASE_GRIDSIZE, y*ASE_GRIDSIZE, 
			   x*ASE_GRIDSIZE+ASE_GRIDSIZE+1,
			   y*ASE_GRIDSIZE+ASE_GRIDSIZE+1);

	pDC->FillSolidRect(rect, cr);
	pDC->FrameRect(&rect, &br);
}

void CAseView::DrawCircle(int x, int y, CDC *pDC, COLORREF cr)
{
	CBrush br(cr), *oldbrush;
	CPen pen(PS_SOLID, 0, RGB(0,0,0)), *oldpen;
	CRect rect(x*ASE_GRIDSIZE, y*ASE_GRIDSIZE, 
			   x*ASE_GRIDSIZE+ASE_GRIDSIZE+1,
			   y*ASE_GRIDSIZE+ASE_GRIDSIZE+1);

	oldbrush = pDC->SelectObject(&br);
	oldpen   = pDC->SelectObject(&pen);

	pDC->Ellipse(rect);

	pDC->SelectObject(oldbrush);
	pDC->SelectObject(oldpen);
}

void CAseView::DrawRoute(CDC *pDC)
{
	CAseDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	CPen pen(PS_SOLID,1,RGB(0,0,0)),*oldpen;

	oldpen = pDC->SelectObject(&pen);

	CAStar *astar = pDoc->GetPathFinder();
	_asNode *best = astar->GetBestNode();
		
	int x,y,px,py;

	if (best) {
		x = best->x;
		y = best->y;
		
		px = x * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);
		py = y * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);

		pDC->MoveTo(px,py);
		best = best->parent;
	}

	while (best) {
		x = best->x;
		y = best->y;
		
		px = x * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);
		py = y * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);
		
		pDC->LineTo(px,py);

		best = best->parent;
	}

	pDC->SetPixel(px,py, RGB(0,0,128));
	pDC->SelectObject(oldpen);
}

void CAseView::HighlightNode(_asNode *node, bool drawKidsConnections)
{
	if (!node) {
		m_cHilight.x = -1;
		m_cHilightNode = NULL;
	} else {
		m_cHilight.x = node->x;
		m_cHilight.y = node->y;
		m_cHilightNode = node;
	}

	Invalidate(false);
}

void CAseView::RemoveHighlight()
{
	m_cHilight.x = -1;
	m_cHilightNode = NULL;
}

void CAseView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	m_cHilight.x = -1;
	m_cHilightNode = NULL;
	GetDocument()->GetStartEnd(m_cStart, m_cEnd);
}

/////////////////////////////////////////////////////////////////////////////
// CAseView diagnostics

#ifdef _DEBUG
void CAseView::AssertValid() const
{
	CView::AssertValid();
}

void CAseView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAseDoc* CAseView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAseDoc)));
	return (CAseDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAseView message handlers

void CAseView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CAseDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	MouseToPoint(point, m_uBrushType);

	if (m_uBrushType < 4) m_bDragging = true;

	CView::OnLButtonDown(nFlags, point);
}

void CAseView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_bDragging = false;
	GetDocument()->NotifyClick();
	
	CView::OnLButtonUp(nFlags, point);
}

void CAseView::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bDragging) {
		MouseToPoint(point, m_uBrushType);
	}
	
	CView::OnMouseMove(nFlags, point);
}

void CAseView::MouseToPoint(CPoint point, UINT brush)
{
	CClientDC dc(this);

	int px = point.x/8, py = point.y/8;

	CPoint round(px*ASE_GRIDSIZE, py*ASE_GRIDSIZE),temp;

	GetDocument()->GetStartEnd(m_cStart, m_cEnd);

	// If start or end
	if (brush == 4) {
		temp.x = m_cStart.x * ASE_GRIDSIZE;
		temp.y = m_cStart.y * ASE_GRIDSIZE;

		m_cStart.x = px;
		m_cStart.y = py;

		InvalidateRect(CRect(round,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
		InvalidateRect(CRect(temp,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
	} else if (brush == 5) {
		temp.x = m_cEnd.x * ASE_GRIDSIZE;
		temp.y = m_cEnd.y * ASE_GRIDSIZE;

		m_cEnd.x = px;
		m_cEnd.y = py;
		
		InvalidateRect(CRect(round,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
		InvalidateRect(CRect(temp,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
	} else if (brush == -1) {
		CPoint bp = GetDocument()->GetBreakpoint();

		temp.x = bp.x * ASE_GRIDSIZE;
		temp.y = bp.y * ASE_GRIDSIZE;

		bp.x = px;
		bp.y = py;

		GetDocument()->SetBreakpoint(bp);

		InvalidateRect(CRect(round,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
		InvalidateRect(CRect(temp,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
	} else {
		char *board = GetDocument()->GetBoard();

		board[ci(px,py)] = brush;
		InvalidateRect(CRect(round,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
	}

	GetDocument()->SetStartEnd(m_cStart, m_cEnd);
}

BOOL CAseView::OnEraseBkgnd(CDC* pDC) 
{
	return false;	
	return CView::OnEraseBkgnd(pDC);
}

void CAseView::OnRButtonDblClk(UINT nFlags, CPoint point) 
{
	MouseToPoint(point, -1);
	
	CView::OnRButtonDblClk(nFlags, point);
}
