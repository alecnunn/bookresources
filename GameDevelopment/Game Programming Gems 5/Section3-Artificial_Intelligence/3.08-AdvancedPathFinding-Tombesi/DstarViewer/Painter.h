/**********************************************************************

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2003
 **********************************************************************/
#pragma once

#include "../DStar.h"

//---------------------------------------------------------------------------
//COLORS used
extern COLORREF crBrushes [];

enum colors
{
	COL_grid,
	COL_empty,
	COL_obstacle,
	COL_end,
	COL_start,
	COL_path,
	COL_proc
};
//---------------------------------------------------------------------------

class Painter
{
	unsigned m_quad_per_side;
	long	 m_pix_per_quad;
	long     m_epsilon;
	CPoint	 m_offset;
	bool	 m_draw_processed_nodes;

public:
	Painter(void);
	~Painter(void);

	void Reset(unsigned quad_per_side, CWnd* wnd);

	CRect GetPixRect(unsigned r, unsigned c);
	bool GetNodeCoords(CPoint pixPoint, unsigned &r, unsigned &c);
	CPoint GetPixelCoord(point p);
	bool GetEdgeCoord(CPoint p, point &p0, point &p1);
	void DrawRectangle(unsigned r, unsigned c, unsigned lato, CDC *pDC, COLORREF crFill, COLORREF crBorder);
	void DrawRectangleNB(unsigned r, unsigned c, unsigned lato, CDC *pDC, COLORREF crFill);
	void DrawCircle(unsigned r, unsigned c, CDC *pDC, COLORREF cr);
	void DrawEdge(point p0, point p1, CDC *pDC, COLORREF crColor);
	void DrawGrid(CDC *pDC, COLORREF cr, DStar* d);
	void DrawNodeInfo(CDC *pDC, node_info info);
	void DrawStart(CDC *pDC, point p);
	void DrawGoal(CDC *pDC, point p);
	void DrawCurrent(CDC *pDC, point p);
	inline void SetProcNodesDraw(bool b)
	{this->m_draw_processed_nodes = b;}
};
