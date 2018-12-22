/**********************************************************************

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2003
 **********************************************************************/
#include "StdAfx.h"
#include "painter.h"
#include "ChildView.h"
#include "../DStar.h"

//---------------------------------------------------------------------------
COLORREF crBrushes [] = 
{
	RGB(  0,  0,  0),	// grid
	RGB(240,240,240),	// empty
	RGB(180,180,180),	// obstacle
	RGB(255,  0,  0),	// end	
	RGB(  0,255,  0),	// start
	RGB(255,255,  0),	// path
	RGB(100,100,200)	// processed

};

//---------------------------------------------------------------------------
Painter::Painter(void)
{
	m_quad_per_side			= 0;
	m_pix_per_quad			= 0;
	m_draw_processed_nodes	= true;
	m_offset				= 0;
}

Painter::~Painter(void)
{
}

void Painter::Reset(unsigned quad_per_side, CWnd* wnd)
{
	ASSERT(wnd);

	if (quad_per_side==0)
	{
		m_quad_per_side=0;
		m_pix_per_quad=0;
		m_offset = CPoint(0,0);
		return ;
	}

	CRect r;
	wnd->GetClientRect(&r);
	CSize pixSize = r.Size();
	unsigned pix_p_quad = min(pixSize.cx,pixSize.cy) / quad_per_side;

	m_quad_per_side=quad_per_side;
	m_pix_per_quad=pix_p_quad;
	m_epsilon = m_pix_per_quad / 3;
	m_offset = CPoint(m_pix_per_quad/2, m_pix_per_quad/2);
}

CRect Painter::GetPixRect(unsigned r, unsigned c)
{
	CRect rect;
	{
		rect= CRect(	(c)*m_pix_per_quad,
						(r)*m_pix_per_quad,
						(c)*m_pix_per_quad,
						(r)*m_pix_per_quad	);
	}
	rect += m_offset;
	return rect;
}

void Painter::DrawEdge(point p0, point p1, CDC *pDC, COLORREF crColor)
{
	CPoint start, end;
	CPen pen(PS_SOLID, 2, crColor);
	
	int sz = m_pix_per_quad/5;
	start = GetPixelCoord(p0);
	end   = GetPixelCoord(p1);
	CPen* oldpen = pDC->SelectObject(&pen);
	if (p0.y < p1.y)
	{
		//left --> right draw
		start.x +=sz-1;
		end.x   -=sz;
	}
	else
	{
		//up --> down draw
		start.y +=sz-1;
		end.y	-=sz;
	}
	pDC->MoveTo(start);
	pDC->LineTo(end);
	pDC->SelectObject(oldpen);
}

CPoint Painter::GetPixelCoord(point p)
{
	CPoint out;

	out.x = (p.y)*m_pix_per_quad;
	out.y = (p.x)*m_pix_per_quad;
	out += m_offset;
	return out;
}

bool Painter::GetEdgeCoord(CPoint p, point &p0, point &p1)
{
	CPoint quadrant;

    if(!m_pix_per_quad)
	{
		p0.x=p0.y=p1.x=p1.y=0;
		return false;
	}
	p -= m_offset;
	if ((p.x < 0) || (p.x > m_pix_per_quad*m_quad_per_side) 
		|| (p.y < 0) || (p.y > m_pix_per_quad*m_quad_per_side))
	{
		p0.x=p0.y=p1.x=p1.y=0;
		return false;
	}
	quadrant.x = p.y / m_pix_per_quad;
	quadrant.y = p.x / m_pix_per_quad;
	if ((p.x <= quadrant.y*m_pix_per_quad + m_epsilon) && 
	    (p.y >= quadrant.x*m_pix_per_quad + m_epsilon) && 
		(p.y <= quadrant.x*m_pix_per_quad + m_pix_per_quad - m_epsilon))
	{
		//Ovest
		p0.x = quadrant.x;
		p0.y = quadrant.y;
		p1.x = p0.x + 1;
		p1.y = p0.y;
		return true;
	}
	if ((p.y <= quadrant.x*m_pix_per_quad + m_epsilon) && 
	    (p.x >= quadrant.y*m_pix_per_quad + m_epsilon) && 
		(p.x <= quadrant.y*m_pix_per_quad + m_pix_per_quad - m_epsilon))
	{
		//North
		p0.x = quadrant.x;
		p0.y = quadrant.y;
		p1.x = p0.x;
		p1.y = p0.y + 1;
		return true;
	}
	if ((p.x >= quadrant.y*m_pix_per_quad + m_pix_per_quad - m_epsilon) && 
	    (p.y >= quadrant.x*m_pix_per_quad + m_epsilon) && 
		(p.y <= quadrant.x*m_pix_per_quad + m_pix_per_quad - m_epsilon))
	{
		//Est
		p0.x = quadrant.x;
		p0.y = quadrant.y + 1;
		p1.x = p0.x + 1;
		p1.y = p0.y;
		return true;
	}
	if ((p.y >= quadrant.x*m_pix_per_quad + m_pix_per_quad - m_epsilon) && 
	    (p.x >= quadrant.y*m_pix_per_quad + m_epsilon) && 
		(p.x <= quadrant.y*m_pix_per_quad + m_pix_per_quad - m_epsilon))
	{
		//South
		p0.x = quadrant.x + 1;
		p0.y = quadrant.y;
		p1.x = p0.x;
		p1.y = p0.y + 1;
		return true;
	}
	return false;
}

void Painter::DrawCircle(unsigned r, unsigned c, CDC *pDC, COLORREF cr)
{
	CBrush br(cr);
	CPen pen(PS_SOLID, 0, RGB(0,0,0));
	int sz = m_pix_per_quad/5;
	
	CRect rect = GetPixRect(r,c);
	rect.InflateRect(sz, sz);
	pDC->SelectObject(&br);
	pDC->SelectObject(&pen);
	pDC->Ellipse(rect);
}

void Painter::DrawGrid(CDC *pDC, COLORREF cr, DStar* d)
{
	for (unsigned i=0; i<m_quad_per_side; i++)
	{
		for (unsigned j=0; j<m_quad_per_side; j++)
		{
			point p0, p1;

			p0.x = i;
			p0.y = j;
			p1.x = i+1;
			p1.y = j;
			if (i < m_quad_per_side-1)
			{	
				unsigned long value = d->GetEdgeCost(p0, p1);

				if (value == Edge_Clear)
					DrawEdge(p0, p1, pDC, crBrushes[COL_grid]);
				if (value == Edge_Obstacle)
					DrawEdge(p0, p1, pDC, crBrushes[COL_empty]);
			}
			p1.x = i;
			p1.y = j+1;
			if (j < m_quad_per_side-1)  
			{
				unsigned long value = d->GetEdgeCost(p0, p1);

				if (value == Edge_Clear)
					DrawEdge(p0, p1, pDC, crBrushes[COL_grid]);
				if (value == Edge_Obstacle)
					DrawEdge(p0, p1, pDC, crBrushes[COL_empty]);
			}
			point p;
			p.x = i;
			p.y = j;
			if ((pDC->GetPixel(GetPixelCoord(p)) != crBrushes[COL_start]) &&
				(pDC->GetPixel(GetPixelCoord(p)) != crBrushes[COL_end]))
				DrawCircle(i, j, pDC, crBrushes[COL_empty]);
		}
	}
	if ( (d) && (this->m_draw_processed_nodes) )
	{//draw processed nodes
		point* nodes = d->GetProcessedNodes();
		for (unsigned i=0; i < d->NodesProcessedSinceLast(); i++)
			DrawCircle(nodes[i].x, nodes[i].y, pDC, crBrushes[COL_proc]);
	}
	
}
void Painter::DrawStart(CDC *pDC, point p)
{ DrawCircle(p.x, p.y, pDC, crBrushes[COL_start]); }

void Painter::DrawGoal(CDC *pDC, point p)
{ DrawCircle(p.x, p.y, pDC, crBrushes[COL_end]); }

void Painter::DrawCurrent(CDC *pDC, point p)
{ DrawCircle(p.x, p.y, pDC, crBrushes[COL_path]); }
