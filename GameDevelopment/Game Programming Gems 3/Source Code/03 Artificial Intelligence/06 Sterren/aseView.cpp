/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */

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

//! \todo remove first 4 entries
static COLORREF crBrushes [] = {
	RGB(255,000,000), // start
	RGB(000,255,000)  // end
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


void CAseView::DisplayWaitCursor()
  {
    BeginWaitCursor();
  }


void CAseView::DisplayDefaultCursor()
  {
    EndWaitCursor();
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
	
	memdc.FillSolidRect(rect, RGB(255,255,255));

	int  i;
	int  dx = rect.Width();
	int  dy = rect.Height();
	bool bSearch    = pDoc->DoesDisplaySearchSpace();
  bool bThreat    = pDoc->DoesDisplayThreatPositions();
  bool bLOF       = pDoc->DoesDisplayThreatLinesOfFire();
  bool bApproxLOF = pDoc->DoesDisplayApproximatedLinesOfFire();

	// Cycle through the board and draw any squares
  const ASE_TerrainBoard*          terrain;
  const ASE_ThreatBoard*           threats;
  const ASE_SearchBoard*           searchspace;
  const ASE_LOSApproximationBoard* losapprox;
  terrain     = pDoc->GetTerrainBoard();
  threats     = pDoc->GetThreatBoard();
  searchspace = pDoc->GetSearchSpaceBoard();
  losapprox   = pDoc->GetLOSApproximationBoard();

  // first draw non-boxed contents (lof, search space)
  unsigned int row;
  unsigned int col;
  for ( row = 0; row < terrain->GetNumberOfRows(); ++row )
    {
      for ( col = 0; col < terrain->GetNumberOfColumns(); ++col )
        {
          unsigned int idx;
          idx = terrain->GetIndexForRowColumn(row, col);

          // ignore boxed terrain here 
          if ( !terrain->IsEmptyCell(idx) )
            continue;

          ASE_Board::CellValue threattype;
          // clip to threat lof
          threattype = min(1, threats->GetCellValue(idx));

          if (   ( bSearch ) 
              && ( !searchspace->IsEmptyCell(idx) )
             )
            {
              // empty terrain, so solely look at lof
              if ( bApproxLOF )
                {
                  threattype  = losapprox->GetCellValue(idx);
                  threattype  = min(2, threattype);
                }
              else
              if ( !bLOF )
                {
                  threattype = 0;
                }
              DrawRectangleNB(row, col, &memdc, 
                              searchspace->GetCellColorForOffset(idx, threattype) 
                             );
            }
          else 
          if (   ( bApproxLOF ) 
              && ( !losapprox->IsEmptyCell(idx) )
             )
            {
              // empty terrain, only display approx lof here (even if it might be a threat position)
              DrawRectangleNB(row, col, &memdc, losapprox->GetCellColorForOffset(idx, 0));
            }
          else
          if (   ( bLOF ) 
              && ( !threats->IsEmptyCell(idx) )
             )
            {
              // empty terrain, only display lof here (even if it might be a threat position)
              DrawRectangleNB(row, col, &memdc, threats->GetCellColorForOffset(idx, 0));
            }
        }
    }

  for ( row = 0; row < terrain->GetNumberOfRows(); ++row )
    {
      for ( col = 0; col < terrain->GetNumberOfColumns(); ++col )
        {
          unsigned int idx;
          idx = terrain->GetIndexForRowColumn(row, col);

          // ignore non-boxed terrain here 
          if (   ( terrain->IsEmptyCell(idx) )
              && ( !threats->IsThreatPosition(idx) )
             )
            continue;

          if (   ( bSearch ) 
              && ( !searchspace->IsEmptyCell(idx) )
             )
            {
              ASE_Board::CellValue terraintype;
              terraintype = terrain->GetCellValue(idx);

              ASE_Board::CellValue threattype;
              threattype = threats->GetCellValue(idx);
              if (   ( threattype != 0 )
                  && (   (   ( !bLOF )
                          && ( !threats->IsThreatPosition(idx) )
                         )
                      || (   ( !bThreat )
                          && ( threats->IsThreatPosition(idx) )
                         )
                     )
                 )
                {
                  threattype = 0;
                }
              else
                {
                  threattype = 1;
                }
              
              ASE_Board::CellValue offset;
              offset = max(terraintype, threattype);

              DrawRectangleNB(row, col, &memdc, 
                              searchspace->GetCellColorForOffset(idx, offset) 
                             );
            }
          else
          if (   ( !threats->IsEmptyCell(idx) )
              && (   (   ( bThreat ) 
                      && ( threats->IsThreatPosition(idx) )
                     )
                  || (   ( bLOF )
                      && ( !terrain->IsEmptyCell(idx) )
                     )  
                 )
             )
            {
              ASE_Board::CellValue terraintype;
              terraintype = terrain->GetCellValue(idx);

              DrawRectangle(row, col, &memdc, 
                            threats->GetCellColorForOffset(idx, terraintype) 
                           );
            }
          else
          if ( !terrain->IsEmptyCell(idx) )
            {
				      DrawRectangle(row, col, &memdc, terrain->GetCellColor(idx));
            }
        }
    }

	// Draw start, end and debug points
	if (m_cStart.x != -1) DrawRectangle(m_cStart.x, m_cStart.y, &memdc, crBrushes[0]);
	if (m_cEnd.x != -1)   DrawRectangle(m_cEnd.x, m_cEnd.y, &memdc, crBrushes[1]);

  // set pen for grid
	CPen  gridpen(PS_SOLID, 0, RGB(192, 192, 192));
  CPen* oldgridpen;

	oldgridpen   = memdc.SelectObject(&gridpen);


	// Draw the grid...
	for (i=0; i<dx; i+=8) 
    {
		  for (int j=0;j<dy;j+=8) 
        {
			    memdc.SetPixel(i, j, RGB(192, 192, 192));

          if (   ( 0 == (i % 64) )
              && ( 0 == (j % 64) )
             )
            {
			        memdc.MoveTo(i - 2, j);
			        memdc.LineTo(i + 3, j);
			        memdc.MoveTo(i, j - 2);
			        memdc.LineTo(i, j + 3);
            }
		    }
	  }
	memdc.SelectObject(oldgridpen);

	// Draw route if necessary
	if ( pDoc->DoesDisplayRoute() ) 
    DrawRoute(&memdc);

	if (m_cHilight.x != -1) {
		DrawRectangle(m_cHilight.x, m_cHilight.y, &memdc, RGB(0,0,255));
		
		CPoint centre;
		centre.x = m_cHilight.x*ASE_GRIDSIZE+(ASE_GRIDSIZE/2);
		centre.y = m_cHilight.y*ASE_GRIDSIZE+(ASE_GRIDSIZE/2);

		memdc.MoveTo(centre);

		unsigned int numchildren = m_cHilightNode->GetNumberOfChildren();
		_asNode *temp;
	
		for ( unsigned int i=0; i<numchildren; i++ ) 
      {
			  temp = m_cHilightNode->GetChild(i);
			  memdc.LineTo(temp->GetX() * ASE_GRIDSIZE + (ASE_GRIDSIZE/2),
				             temp->GetY() * ASE_GRIDSIZE + (ASE_GRIDSIZE/2));
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

void CAseView::DrawRectangleNB(int x, int y, CDC *pDC, COLORREF cr)
{
	CRect rect(x*ASE_GRIDSIZE, y*ASE_GRIDSIZE, 
			   x*ASE_GRIDSIZE+ASE_GRIDSIZE+1,
			   y*ASE_GRIDSIZE+ASE_GRIDSIZE+1);

	pDC->FillSolidRect(rect, cr);
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
		x = best->GetX();
		y = best->GetY();
		
		px = x * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);
		py = y * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);

		pDC->MoveTo(px,py);
		best = best->GetParent();
	}

	while (best) {
		x = best->GetX();
		y = best->GetY();
		
		px = x * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);
		py = y * ASE_GRIDSIZE + (ASE_GRIDSIZE / 2);
		
		pDC->LineTo(px,py);

		best = best->GetParent();
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
		m_cHilight.x = node->GetX();
		m_cHilight.y = node->GetY();
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
	} 
  else if (brush == -1) 
  {
		CPoint bp = GetDocument()->GetBreakpoint();

		temp.x = bp.x * ASE_GRIDSIZE;
		temp.y = bp.y * ASE_GRIDSIZE;

		bp.x = px;
		bp.y = py;

		GetDocument()->SetBreakpoint(bp);

		InvalidateRect(CRect(round,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
		InvalidateRect(CRect(temp,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
	} 
  else if (brush == 6) // threat position 
  {
    GetDocument()->GetThreatBoard()->MarkThreatPosition(px, py);
    GetDocument()->GetLOSApproximationBoard()->ApplyThreatModification(px, py);

    if ( GetDocument()->DoesDisplayApproximatedLinesOfFire() )
      {
        DisplayWaitCursor();

        GetDocument()->GetLOSApproximationBoard()->ComputeLinesOfFireApproximation();

        DisplayDefaultCursor();
      }

    //! \todo make a bit smarter, since only the rectangles within a radius are affected
    Invalidate();
	}
  else if (brush == 0) // clearing position
  {
    // check whether threats are visible and if threat needs to be removed (first)
    if (   ( GetDocument()->DoesDisplayThreatPositions() )
        && ( GetDocument()->GetThreatBoard()->IsThreatPosition(px, py) )
       )
      {
        GetDocument()->GetThreatBoard()->ClearThreatPosition(px, py);
        GetDocument()->GetLOSApproximationBoard()->ApplyThreatModification(px, py);

        if ( GetDocument()->DoesDisplayApproximatedLinesOfFire() )
          {
            DisplayWaitCursor();

            GetDocument()->GetLOSApproximationBoard()->ComputeLinesOfFireApproximation();

            DisplayDefaultCursor();
          }

        //! \todo make a bit smarter, since only the rectangles within a radius are affected
        Invalidate();
      }
    else
      {
        bool bFullRedraw;
        bFullRedraw = GetDocument()->GetTerrainBoard()->IsLocationImpassable(px, py);

        GetDocument()->GetTerrainBoard()->SetCellValue(px, py, 0);
        
        if ( bFullRedraw )
          {
            DisplayWaitCursor();

            GetDocument()->GetThreatBoard()->ComputeLinesOfFire();

            GetDocument()->GetLOSApproximationBoard()->ApplyTerrainModification(px, py);

            if ( GetDocument()->DoesDisplayApproximatedLinesOfFire() )
              {
                GetDocument()->GetLOSApproximationBoard()->ComputeLinesOfFireApproximation();
              }

            DisplayDefaultCursor();

            Invalidate();
          }
        else
          {
    		    InvalidateRect(CRect(round,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
          } 
      }
	}
  else if (brush == 3) // impassable terrain
  {
    // clear threat, if necessary 
    GetDocument()->GetTerrainBoard()->SetCellValue(px, py, 3);

    GetDocument()->GetThreatBoard()->ClearThreatPosition(px, py);

    GetDocument()->GetLOSApproximationBoard()->ApplyTerrainModification(px, py);

    if ( GetDocument()->DoesDisplayApproximatedLinesOfFire() )
      {
        DisplayWaitCursor();

        GetDocument()->GetLOSApproximationBoard()->ComputeLinesOfFireApproximation();

        DisplayDefaultCursor();
      }

    Invalidate();
  }
  else // terrain weights
  {
    //! \todo handle invalidation after painting impassable terrain, since
    //        it may block lof
    GetDocument()->GetTerrainBoard()->SetCellValue(px, py, brush);

    GetDocument()->GetLOSApproximationBoard()->ApplyTerrainModification(px, py);

    if ( GetDocument()->DoesDisplayApproximatedLinesOfFire() )
      {
        DisplayWaitCursor();

        GetDocument()->GetLOSApproximationBoard()->ComputeLinesOfFireApproximation();

        DisplayDefaultCursor();
      }

    assert( GetDocument()->GetTerrainBoard()->GetCellValue(px, py) == brush );
		InvalidateRect(CRect(round,CSize(ASE_GRIDSIZE+1,ASE_GRIDSIZE+1)), false);
	}

	GetDocument()->SetModifiedFlag();
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
