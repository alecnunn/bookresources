/**********************************************************************

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2003
 **********************************************************************/
// ChildView.cpp : implementation of the CChildView class
//

#include "stdafx.h"
#include "DstarViewer.h"
#include "ChildView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_operation_state = OP_Idle;
	m_quad_cnt		  = 0;
	m_pDStar		  = 0;
	m_timer_play	  = 0;
}

CChildView::~CChildView()
{
	OnOperationsClearmap();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_COMMAND(ID_OPERATIONS_SETUPMAP, OnSetBase)
	ON_COMMAND(ID_OPERATIONS_CLEARMAP, OnOperationsClearmap)
	ON_COMMAND(ID_OPERATIONS_SETOBSTACLE, OnOperationsSetobstacle)
	ON_COMMAND(ID_OPERATIONS_SETCLEAR, OnOperationsSetclear)
	ON_COMMAND(ID_OPERATIONS_PLAY, OnOperationsPlay)
	ON_COMMAND(ID_OPERATIONS_STEP, OnOperationsStep)
ON_WM_MOUSEMOVE()
ON_WM_RBUTTONDOWN()
ON_WM_LBUTTONDOWN()
ON_WM_TIMER()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);

	return TRUE;
}



void CChildView::OnPaint() 
{	
	CPaintDC dc(this); // device context for painting	

	m_Painter.Reset(m_quad_cnt, this);	

	m_Painter.DrawGrid(&dc, crBrushes[COL_grid], m_pDStar);
		
	if (m_pDStar)
	{
		point p = m_pDStar->GetStart();
		m_Painter.DrawStart(&dc, p);
		p = m_pDStar->GetGoal();
		m_Painter.DrawGoal(&dc, p);
		p = m_pDStar->CurrentLocation();
		m_Painter.DrawCurrent(&dc, p);
	}
}

void CChildView::OnOperationsSetupmap()
{
	OnOperationsClearmap();

	point start;	
	start.x = 0;
	start.y = 0;
	point goal;
	goal.x = m_quad_cnt-1;
	goal.y = m_quad_cnt-1;

	m_pDStar = new DStar(m_quad_cnt, start, goal);

	for (unsigned i =0; i<m_quad_cnt; i++)
	{
		for (unsigned j =0; j<m_quad_cnt; j++)
		{
			point p0, p1;

			p0.x = i;
			p0.y = j;
			p1.x = i+1;
			p1.y = j;
			
			if (i < m_quad_cnt - 1) m_pDStar->SetEdge(p0, p1, 1);
			p1.x = i;
			p1.y = j+1;
			if (j < m_quad_cnt - 1) m_pDStar->SetEdge(p0, p1, 1);
		}
	}
	
	Invalidate(FALSE);
}

void CChildView::OnOperationsClearmap()
{
	if (m_pDStar)
	{
		delete m_pDStar;
		m_pDStar=0;
	}
	if (m_timer_play)
	{
		this->KillTimer(m_timer_play);
		m_timer_play =0;
	}

	m_operation_state = OP_Idle;
	m_quad_cnt = 0;
	Invalidate();
}


void CChildView::OnOperationsSetobstacle()
{
	m_Painter.SetProcNodesDraw(true);
	if (m_pDStar)
	{
		point curr, goal;
		curr = m_pDStar->CurrentLocation();
		goal = m_pDStar->GetGoal();
		if (( curr.x == goal.x ) && (curr.y == goal.y))
		{
			AfxMessageBox("Agent is already at goal!", MB_OK);
			return;
		}
	}
	m_operation_state = OP_Obstacle;
}

void CChildView::OnOperationsPlay()
{
	m_Painter.SetProcNodesDraw(false);
	if (m_pDStar)
	{
		point curr, goal;
		curr = m_pDStar->CurrentLocation();
		goal = m_pDStar->GetGoal();
		if (( curr.x == goal.x ) && (curr.y == goal.y))
		{
			AfxMessageBox("Agent is already at goal!", MB_OK);
			return;
		}
	}
	m_timer_play = this->SetTimer(1, 1000, NULL);
}

void CChildView::OnOperationsStep()
{
	long len;

	if (m_pDStar)
	{
		point curr, goal;
		curr = m_pDStar->CurrentLocation();
		goal = m_pDStar->GetGoal();
		if (( curr.x == goal.x ) && (curr.y == goal.y))
		{
			AfxMessageBox("Agent is already at goal!", MB_OK);
			return;
		}
	}
	m_operation_state = OP_Step;
	m_Painter.SetProcNodesDraw(false);
	if (m_pDStar)
	{
		point curr, goal;

		m_pDStar->Play(1, len);
		curr = m_pDStar->CurrentLocation();
		goal = m_pDStar->GetGoal();
		if (( curr.x == goal.x ) && (curr.y == goal.y))
			AfxMessageBox("Agent arrived at goal!", MB_OK);
	}
	Invalidate(FALSE);
}

void CChildView::OnOperationsSetclear()
{
	m_Painter.SetProcNodesDraw(true);
	if (m_pDStar)
	{
		point curr, goal;
		curr = m_pDStar->CurrentLocation();
		goal = m_pDStar->GetGoal();
		if (( curr.x == goal.x ) && (curr.y == goal.y))
		{
			AfxMessageBox("Agent is already at goal!", MB_OK);
			return;
		}
	}
	m_operation_state = OP_Clear;
}


static unsigned  NodeValueByState(OperationState state)
{
	if (state==OP_Obstacle)
		return Edge_Obstacle;
	if (state==OP_Clear)
		return Edge_Clear;
	if (state==OP_Start)
		return Edge_Clear;
	if (state==OP_End)
		return Edge_Clear;

	else
		return 0;
}

void CChildView::OnMouseMove(UINT nFlags, CPoint point)
{
	if (OP_Idle!=m_operation_state && MK_LBUTTON==nFlags)
	{
		::point p0, p1;

		if (m_pDStar && m_Painter.GetEdgeCoord(point, p0, p1))
		{
			m_pDStar->SetEdge(p0, p1, NodeValueByState(m_operation_state));
			Invalidate(FALSE);
		}
	}

	CWnd::OnMouseMove(nFlags, point);}

void CChildView::OnRButtonDown(UINT nFlags, CPoint point)
{
	m_operation_state = OP_Idle;

	if(m_timer_play)
	{
		this->KillTimer(m_timer_play);
		m_timer_play=0;
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	if (OP_Idle!=m_operation_state)
	{
		//set state to cell
		::point p0, p1;

		if (m_pDStar && m_Painter.GetEdgeCoord(point, p0, p1))
		{
			m_pDStar->SetEdge(p0, p1, NodeValueByState(m_operation_state));
			Invalidate(FALSE);
		}
	}
	CWnd::OnLButtonDown(nFlags, point);
}

void CChildView::OnTimer(UINT nIDEvent)
{
	if (nIDEvent==1)
	{
		if (m_pDStar)
		{
			point curr, goal;

			curr = m_pDStar->CurrentLocation();
			goal = m_pDStar->GetGoal();
			if (( curr.x == goal.x ) && (curr.y == goal.y))
			{
				m_operation_state = OP_Idle;
				this->KillTimer(m_timer_play);
				m_timer_play=0;
				AfxMessageBox("Agent is arrived at goal!", MB_OK);
				return;
			}
			long len;
			m_pDStar->Play(1, len);
		}
		Invalidate(FALSE);
	}
	CWnd::OnTimer(nIDEvent);
}

// App command to run the dialog
void CChildView::OnSetBase()
{
	OnOperationsClearmap();
	SetBaseDim * set_base_dim = new SetBaseDim();
    m_quad_cnt = set_base_dim->DoModal();
	delete set_base_dim;
	if (!m_quad_cnt) m_quad_cnt = 16; //Default
	point start;	
	start.x = 0;
	start.y = 0;
	point goal;
	goal.x = m_quad_cnt-1;
	goal.y = m_quad_cnt-1;

	m_pDStar = new DStar(m_quad_cnt, start, goal);

	for (unsigned i =0; i<m_quad_cnt; i++)
	{
		for (unsigned j =0; j<m_quad_cnt; j++)
		{
			point p0, p1;

			p0.x = i;
			p0.y = j;
			p1.x = i+1;
			p1.y = j;
			
			if (i < m_quad_cnt - 1) m_pDStar->SetEdge(p0, p1, 1);
			p1.x = i;
			p1.y = j+1;
			if (j < m_quad_cnt - 1) m_pDStar->SetEdge(p0, p1, 1);
		}
	}
	
	Invalidate(FALSE);
}


//SetBaseDim Dialog
SetBaseDim::SetBaseDim():CDialog(SetBaseDim::IDD)
{
}

void SetBaseDim::OnOK()
{
	TCHAR buff[1024];
	
	CEdit* pEdit = (CEdit*) this->GetDlgItem(IDC_GETSIZE);
	pEdit->GetWindowText(&buff[0], 5);
	int base = atoi(&buff[0]);
	this->EndDialog(base);
	//CDialog::OnOK();
}

void SetBaseDim::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(SetBaseDim, CDialog)
END_MESSAGE_MAP()