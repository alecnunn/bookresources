// aseDoc.cpp : implementation of the CAseDoc class
//

#include "stdafx.h"
#include "ase.h"

#include "nodeview.h"

#include "aseDoc.h"
#include "aseView.h"
#include "mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAseDoc

IMPLEMENT_DYNCREATE(CAseDoc, CDocument)

BEGIN_MESSAGE_MAP(CAseDoc, CDocument)
	//{{AFX_MSG_MAP(CAseDoc)
	ON_COMMAND(ID_RUNTOBREAKPOINT, OnRunToBreakpoint)
	ON_UPDATE_COMMAND_UI(ID_RUNTOBREAKPOINT, OnUpdateRunToBreakpoint)
	ON_COMMAND(ID_EXECUTEASTAR, OnExecuteAStar)
	ON_COMMAND(ID_STEPASTAR, OnStepAStar)
	ON_COMMAND(ID_PATHING_ALLOWDIAGONAL, OnPathingAllowDiagonal)
	ON_UPDATE_COMMAND_UI(ID_PATHING_ALLOWDIAGONAL, OnUpdatePathingAllowDiagonal)
	ON_COMMAND(ID_VIEW_AROUTE, OnViewARoute)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AROUTE, OnUpdateViewARoute)
	ON_COMMAND(ID_PATHING_CONTINUOUSUPDATE, OnPathingContinuousUpdate)
	ON_UPDATE_COMMAND_UI(ID_PATHING_CONTINUOUSUPDATE, OnUpdatePathingContinuousUpdate)
	ON_COMMAND(ID_PATHING_RELATIVECOSTING, OnPathingRelativeCosting)
	ON_UPDATE_COMMAND_UI(ID_PATHING_RELATIVECOSTING, OnUpdatePathingRelativeCosting)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_WEIGHT0, ID_ENDPOINT, OnBrushType)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_WEIGHT0, ID_ENDPOINT, OnUpdateUIBrushType)	
	ON_COMMAND_RANGE(ID_PATHING_BREAKPOINTS_POINT, ID_PATHING_BREAKPOINTS_NEWCHILD, OnBreakpointType)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_PATHING_BREAKPOINTS_POINT, ID_PATHING_BREAKPOINTS_NEWCHILD, OnUpdateUIBreakpointType)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAseDoc construction/destruction

CAseDoc::CAseDoc()
{
	memset(m_cBoard, 0, sizeof(m_cBoard));
	m_bAllowDiagonal = true;
	m_bContinualUpdate = false;
	m_uBrushType = 0;
	m_bRelativeCosting = false;
}

CAseDoc::~CAseDoc()
{
}

BOOL CAseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	memset(m_cBoard, 0, sizeof(m_cBoard));
	m_cStart.x = -1;
	m_cEnd.x = -1;
	m_cBreakpoint.x = -1;
	m_iBreakData = -1;
	m_bStepped = false;

	m_cAStar.Reset();
	m_bDrawRoute = false;

	return TRUE;
}

BOOL CAseDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;
	
	m_bStepped = false;
	m_cBreakpoint.x = -1;
	m_iBreakData = -1;
	
	m_cAStar.Reset();
	m_bDrawRoute = false;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAseDoc serialization

void CAseDoc::Serialize(CArchive& ar)
{
	int x = ASE_BOARDX, y = ASE_BOARDY;

	if (ar.IsStoring()) {
		ar << x;
		ar << y;
		ar << m_cStart;
		ar << m_cEnd;

		for (int i=0; i<x; i++) {
			for (int j=0; j<y; j++) {
				ar << m_cBoard[i][j];
			}
		}
	} else {
		ar >> x;
		ar >> y;
		ar >> m_cStart;
		ar >> m_cEnd;

		for (int i=0; i<x; i++) {
			for (int j=0; j<y; j++) {
				ar >> m_cBoard[i][j];
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CAseDoc diagnostics

#ifdef _DEBUG
void CAseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CAseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAseDoc commands

int CAseDoc::AS_Valid(_asNode *parent, _asNode *node, int data, void *pointer) 
{
	int x = node->x, y = node->y;
	CAseDoc *me = reinterpret_cast<CAseDoc *>(pointer);

	if (x < 0 || y < 0 || x >= ASE_BOARDX || y >= ASE_BOARDY) return FALSE;
	if (me->m_cBoard[x][y] == 3) return FALSE;

	if (!(me->m_bAllowDiagonal)) {
		int px = parent->x;
		int py = parent->y;

		if (px - x != 0 && py - y != 0) return FALSE;
	}

	return TRUE;
}

int CAseDoc::AS_Cost(_asNode *parent, _asNode *node, int data, void *pointer) 
{
	CAseDoc *me = reinterpret_cast<CAseDoc *>(pointer);

	int cost = me->m_cBoard[node->x][node->y]+1;	// Ensure always cost > 1

	return cost;
}

int CAseDoc::AS_RelativeCost(_asNode *parent, _asNode *node, int data, void *pointer) 
{
	CAseDoc *me = reinterpret_cast<CAseDoc *>(pointer);

	int cost = abs(me->m_cBoard[node->x][node->y] - 
		           me->m_cBoard[parent->x][parent->y])+1;

	return cost;
}

void CAseDoc::NodeAdded(_asNode *node, int data) 
{
	if (m_cBreakpoint.x == -1 && m_iBreakData == -1) return;

	if (node->x == m_cBreakpoint.x && node->y == m_cBreakpoint.y) {
		m_bBreakpointHit = true;
		m_pcBreakNode = node;
	} 
	
	if (data == m_iBreakData) {
		m_bBreakpointHit = true;
		m_pcBreakNode = node;
	}
}

CNodeView *CAseDoc::GetNodeView()
{
	POSITION pos;
	pos = GetFirstViewPosition();

	CNodeView *nv = reinterpret_cast<CNodeView *>(GetNextView(pos));

	return nv;
}

CAseView *CAseDoc::GetAseView()
{
	CMainFrame* pMfm = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
	CAseView *view = reinterpret_cast<CAseView *>(pMfm->GetRightPane());
	
	return view;
}

bool CAseDoc::SetupAStar(bool stepped/* = false*/)
{
	if (m_cStart.x == -1 || m_cEnd.x == -1) {
		MessageBox("Please ensure that both the start and end points are specified.\nA red and green square denote the start and end points, respectively.", "A* Explorer", MB_ICONERROR);
		return false;
	}

	CNodeView *nv = GetNodeView();
	GetAseView()->RemoveHighlight();

	m_cAStar.SetRows(ASE_BOARDX);
	m_cAStar.udValid = AS_Valid;
	m_cAStar.udCost = (m_bRelativeCosting) ? AS_RelativeCost : AS_Cost;
	
	m_cAStar.udNotifyList = NULL;
	m_cAStar.udNotifyChild = (m_bContinualUpdate) ? NULL : CNodeView::OnNotifyChild;

	m_cAStar.m_pCBData = reinterpret_cast<void *>(this);
	m_cAStar.m_pNCData = reinterpret_cast<void *>(nv);

	nv->OnPreAStar();

	if (stepped) {
		CTreeCtrl &tree = GetNodeView()->GetTreeCtrl();
		tree.InsertItem("A* Tree", 6, 6);
		tree.InsertItem("Open List", 7, 7);
		tree.InsertItem("Closed List", 8, 8);

		m_cAStar.udNotifyList = CNodeView::OnNotifyList;
		m_cAStar.StepInitialize(m_cStart.x, m_cStart.y, m_cEnd.x, m_cEnd.y);
	}

	return true;
}

void CAseDoc::DrawNode(_asNode *node)
{
	GetAseView()->HighlightNode(node, true);
}

void CAseDoc::MessageBox(CString title, CString caption, UINT type)
{
	AfxGetMainWnd()->MessageBox(title, caption, type);
}

void CAseDoc::OnBreakpointType(UINT uType)
{
	int type = uType - ID_PATHING_BREAKPOINTS_POINT;

	if (type == 0) {
		m_cBreakpoint.x = -1;
		UpdateAllViews(NULL);
	}

	m_iBreakData = (type == 0) ? -1 : type;
}

void CAseDoc::OnUpdateUIBreakpointType(CCmdUI *pCmdUI)
{
	UINT uID = pCmdUI->m_nID;

	if (uID == ID_PATHING_BREAKPOINTS_POINT) {
		pCmdUI->Enable((m_cBreakpoint.x != -1));
		pCmdUI->SetCheck((m_cBreakpoint.x != -1));
		return;
	}

	pCmdUI->SetCheck(uID == unsigned(m_iBreakData + ID_PATHING_BREAKPOINTS_POINT));
}

void CAseDoc::OnRunToBreakpoint() 
{
	if (!m_bStepped) {
		if (!SetupAStar(true)) return;

		m_bDrawRoute = false;
		m_bStepped = true;

		m_bBreakpointHit = (m_cBreakpoint == m_cStart) ? true : false;
	} else {
		m_bBreakpointHit = false;
	}
	
	int retval = 1;
	while (!m_bBreakpointHit) {
		retval = m_cAStar.Step();
		GetNodeView()->SortOpen();

		if (retval == -1) {
			m_bStepped= false;
			MessageBox("A path could not be found.", "A* Explorer", MB_ICONERROR);
			break;
		} else if (retval == 1) {
			m_bDrawRoute = true;
			m_bStepped = false;
			GetNodeView()->OnPostAStar(m_cAStar.GetBestNode());
			UpdateAllViews(NULL);

			MessageBox("Stepping complete. A path has been found.", "A* Explorer", MB_ICONINFORMATION);
			break;
		}
	}

	if (m_bBreakpointHit) {
		CString str;
		if (m_iBreakData == -1) {
			str.Format("Breakpoint at (%d,%d) hit.", m_cBreakpoint.x, m_cBreakpoint.y);
		} else {
			str = "The conditional breakpoint has been satisfied in this iteration.";
		}

		MessageBox(str, "A* Explorer", MB_ICONSTOP);

		CTreeCtrl &tree = GetNodeView()->GetTreeCtrl();
		tree.EnsureVisible(HTREEITEM(m_pcBreakNode->dataptr));
		tree.SelectItem(HTREEITEM(m_pcBreakNode->dataptr));
		tree.Invalidate();
	}
}

void CAseDoc::OnUpdateRunToBreakpoint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_iBreakData != -1 || m_cBreakpoint.x != -1));
}

void CAseDoc::OnExecuteAStar() 
{
	if (m_bStepped) {
		int retval;
		while (true) {
			retval = m_cAStar.Step();
			if (retval == -1) {
				m_bStepped= false;
				GetNodeView()->SortOpen();
				MessageBox("A path could not be found.", "A* Explorer", MB_ICONERROR);
				break;
			} else if (retval == 1) {
				m_bDrawRoute = true;
				m_bStepped = false;
				GetNodeView()->OnPostAStar(m_cAStar.GetBestNode());
				UpdateAllViews(NULL);

				GetNodeView()->SortOpen();
				MessageBox("Stepping complete. A path has been found.", "A* Explorer", MB_ICONINFORMATION);
				break;
			}
		}

		return;
	}

	if (!SetupAStar()) return;

	if (m_cAStar.GeneratePath(m_cStart.x, m_cStart.y, m_cEnd.x, m_cEnd.y)) {
		m_bDrawRoute = true;
		GetNodeView()->OnPostAStar(m_cAStar.GetBestNode());
	} else {
		m_bDrawRoute = false;

		MessageBox("A path could not be found.", "A* Explorer", MB_ICONERROR);
	}

	UpdateAllViews(NULL);
}

void CAseDoc::OnStepAStar() 
{
	if (!m_bStepped) {
		if (!SetupAStar(true)) return;

		m_bStepped = true;
		m_bDrawRoute = false;

		GetNodeView()->RedrawWindow();
	} else {
		int retval = m_cAStar.Step();
		GetNodeView()->SortOpen();

		if (retval == -1) {
			m_bDrawRoute = false;
			m_bStepped = false;

			MessageBox("A path could not be found.", "A* Explorer", MB_ICONERROR);
		} else if (retval == 1) {
			m_bDrawRoute = true;
			m_bStepped = false;

			GetNodeView()->OnPostAStar(m_cAStar.GetBestNode());

			MessageBox("Stepping complete. A path has been found.", "A* Explorer", MB_ICONINFORMATION);
		}

		UpdateAllViews(NULL);
	}
}

void CAseDoc::OnPathingAllowDiagonal() 
{
	m_bAllowDiagonal = !m_bAllowDiagonal;
}

void CAseDoc::OnUpdatePathingAllowDiagonal(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAllowDiagonal);
}

void CAseDoc::OnBrushType(UINT uType)
{
	m_uBrushType = uType - ID_WEIGHT0;
	
	GetAseView()->SetBrushType(m_uBrushType);
}

void CAseDoc::OnUpdateUIBrushType(CCmdUI *pCmdUI)
{
	UINT uType = pCmdUI->m_nID;

	pCmdUI->SetCheck((m_uBrushType == uType - ID_WEIGHT0));
}

void CAseDoc::OnViewARoute() 
{
	m_bDrawRoute = !m_bDrawRoute;

	UpdateAllViews(NULL);
}

void CAseDoc::OnUpdateViewARoute(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_cAStar.GetBestNode() != NULL));
	pCmdUI->SetCheck(m_bDrawRoute);
}

void CAseDoc::OnPathingContinuousUpdate() 
{
	m_bContinualUpdate = !m_bContinualUpdate;
}

void CAseDoc::OnUpdatePathingContinuousUpdate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bContinualUpdate);
}

void CAseDoc::NotifyClick()
{
	if (m_bContinualUpdate && m_cStart.x != -1 && m_cEnd.x != -1) {
		m_bStepped = false;
		OnExecuteAStar();
	}
}

void CAseDoc::OnPathingRelativeCosting() 
{
	m_bRelativeCosting = !m_bRelativeCosting;
}

void CAseDoc::OnUpdatePathingRelativeCosting(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bRelativeCosting);
}
