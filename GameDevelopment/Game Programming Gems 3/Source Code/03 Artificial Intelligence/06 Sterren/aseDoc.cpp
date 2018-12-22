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

// aseDoc.cpp : implementation of the CAseDoc class
//

#include "stdafx.h"
#include "ase.h"

#include "nodeview.h"

#include "aseDoc.h"
#include "aseView.h"
#include "mainfrm.h"

#include <cmath>

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
	ON_COMMAND(ID_VIEW_AROUTE, OnViewARoute)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AROUTE, OnUpdateViewARoute)
	ON_COMMAND(ID_PATHING_CONTINUOUSUPDATE, OnPathingContinuousUpdate)
	ON_UPDATE_COMMAND_UI(ID_PATHING_CONTINUOUSUPDATE, OnUpdatePathingContinuousUpdate)
	ON_COMMAND(ID_VIEW_SEARCHSPACE, OnViewSearchspace)
	ON_UPDATE_COMMAND_UI(ID_VIEW_SEARCHSPACE, OnUpdateViewSearchspace)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_WEIGHT0, ID_ENEMYPOSITION, OnBrushType)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_WEIGHT0, ID_ENEMYPOSITION, OnUpdateUIBrushType)	
	ON_COMMAND_RANGE(ID_PATHING_BREAKPOINTS_POINT, ID_PATHING_BREAKPOINTS_NEWCHILD, OnBreakpointType)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_PATHING_BREAKPOINTS_POINT, ID_PATHING_BREAKPOINTS_NEWCHILD, OnUpdateUIBreakpointType)	

	ON_COMMAND(ID_VIEW_LOF, OnViewLineOfFire)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOF, OnUpdateUIViewLineOfFire)
	ON_COMMAND(ID_VIEW_THREATS, OnViewThreats)
	ON_UPDATE_COMMAND_UI(ID_VIEW_THREATS, OnUpdateUIViewThreats)
	ON_COMMAND(ID_THREATCOSTS_DISTANCETOTHREAT, OnThreatDistanceBasedCosts)
	ON_UPDATE_COMMAND_UI(ID_THREATCOSTS_DISTANCETOTHREAT, OnUpdateUIThreatDistanceBasedCosts)
	ON_COMMAND(ID_VIEW_APPROX_LOF, OnViewApproximatedLinesOfFire)
	ON_UPDATE_COMMAND_UI(ID_VIEW_APPROX_LOF, OnUpdateUIViewApproximatedLinesOfFire)
	ON_COMMAND(ID_THREATCOSTS_THREATCOUNT, OnThreatCountBasedCosts)
	ON_UPDATE_COMMAND_UI(ID_THREATCOSTS_THREATCOUNT, OnUpdateUIThreatCountBasedCosts)
	ON_COMMAND(ID_THREATCOSTS_APPROXLUT, OnThreatFromApproxTableCosts)
	ON_UPDATE_COMMAND_UI(ID_THREATCOSTS_APPROXLUT, OnUpdateUIThreatFromApproxTableCosts)

	ON_COMMAND_RANGE(ID_MOVEMENT_MANHATTAN, ID_MOVEMENT_DIAGONAL, OnMovementType)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_MOVEMENT_MANHATTAN, ID_MOVEMENT_DIAGONAL, OnUpdateUIMovementType)	
	ON_COMMAND_RANGE(ID_MOVEMENTCOSTS_CELL, ID_MOVEMENTCOSTS_RELATIVE, OnMovementCosts)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_MOVEMENTCOSTS_CELL, ID_MOVEMENTCOSTS_RELATIVE, OnUpdateUIMovementCosts)	

	ON_COMMAND_RANGE(ID_THREATCOSTS_NONE, ID_THREATCOSTS_LOFWITHMEMORY, OnThreatCosts)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_THREATCOSTS_NONE, ID_THREATCOSTS_LOFWITHMEMORY, OnUpdateUIThreatCosts)	
	ON_COMMAND_RANGE(ID_THREATMOVEMENT_0, ID_THREATMOVEMENT_2, OnThreatMovement)	
	ON_UPDATE_COMMAND_UI_RANGE(ID_THREATMOVEMENT_0, ID_THREATMOVEMENT_2, OnUpdateUIThreatMovement)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAseDoc construction/destruction

CAseDoc::CAseDoc()
: m_TerrainBoard(ASE_BOARDX, ASE_BOARDY),
  m_ThreatBoard(ASE_BOARDX, ASE_BOARDY, &m_TerrainBoard),
  m_SearchSpaceBoard(ASE_BOARDX, ASE_BOARDY, &m_TerrainBoard),
  m_LOSApproximationBoard(ASE_BOARDX, ASE_BOARDY, &m_TerrainBoard, &m_ThreatBoard),
  // movement type
  m_bMovementDiagonal(false),
  // movement costs
  m_bMovementCostsVectorBased(false),
  m_bMovementCostsRelative(false),
  // threat costs
  m_bThreatCostsNone(true),
  m_bThreatCostsSimple(false),
  m_bThreatLineOfFireWithAimingCosts(false),
  m_bThreatCountCosts(false),
  m_bDistanceToThreatCosts(false),
  m_bThreatCostsFromApproximatedTable(false),
  // views
  m_bSearchSpace(true),
  m_bThreatPositions(true),
  m_bThreatLinesOfFire(true),
  m_bDrawApproximatedLinesOfFire(false),
  m_bDrawRoute(true)
{
	m_bContinualUpdate        = false;
	m_uBrushType              = 0;
}

CAseDoc::~CAseDoc()
{
}

BOOL CAseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

  m_TerrainBoard.Clear();
  m_ThreatBoard.Clear();
  m_SearchSpaceBoard.Clear();

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

  m_SearchSpaceBoard.Clear();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAseDoc serialization

void CAseDoc::Serialize(CArchive& ar)
{
	int x = ASE_BOARDX, y = ASE_BOARDY;

	if (ar.IsStoring()) 
    {
		  ar << x;
		  ar << y;
		  ar << m_cStart;
		  ar << m_cEnd;

      m_TerrainBoard.WriteBoard(ar);
      // writes threats after original board, so remain compatible
      m_ThreatBoard.WriteBoard(ar);
	  } 
  else 
    {
		  ar >> x;
		  ar >> y;
		  ar >> m_cStart;
		  ar >> m_cEnd;

      m_TerrainBoard.ReadBoard(ar);
      // reads threats after original board, to remain compatible
      m_ThreatBoard.Clear();

      // test whether any threat data might be included - dirty
      CFile* file;
      file = ar.GetFile();

      unsigned long bytesAvailable;
      bytesAvailable = static_cast<unsigned long>(file->GetLength() - file->GetPosition());
      if ( bytesAvailable > 0 )
        {
          m_ThreatBoard.ReadBoard(ar);
        }

      // disable button
      m_bDrawApproximatedLinesOfFire = false;

      m_ThreatBoard.InitializeLinesOfFire();
      m_LOSApproximationBoard.Clear();
	  }
}


void CAseDoc::SetStatusBarText(const char* aText)
  {
  	CMainFrame* pMfm = STATIC_DOWNCAST(CMainFrame, AfxGetMainWnd());
    CStatusBar* statusbar = pMfm->GetStatusBar();

    statusbar->SetPaneText(0, aText);
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
	int x       = node->GetX();
  int y       = node->GetY();
	CAseDoc *me = reinterpret_cast<CAseDoc *>(pointer);

  if ( !me->GetTerrainBoard()->IsValidTerrainMove(parent->GetX(), parent->GetY(), x, y) )
    return false;

  // check for invalid diagonal movement
  if ( !me->DoesUseDiagonalMovement() )
    {
		  int px = parent->GetX();
		  int py = parent->GetY();

      return (   ( px == x )
              || ( py == y )
             );
    }
  else
    return true;
}


void CAseDoc::NodeAdded(_asNode *node, int data) 
{
	if (m_cBreakpoint.x == -1 && m_iBreakData == -1) return;


  //! \todo rewrite to single expression
	if (   node->GetX() == m_cBreakpoint.x 
      && node->GetY() == m_cBreakpoint.y
     ) 
    {
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


void CAseDoc::SelectMovementCostFunctions()
  {
    // set appropriate terrain cost function
    if ( DoesUseRelativeMovementCosts() )
      {
    	  m_cAStar.udTerrainCost      = ASE_Costs::CellBasedRelativeMovementCosts;
        m_cAStar.udTerrainHeuristic = ASE_Costs::CellBasedRelativeMovementHeuristic;
      }
    else
    if ( DoesUseVectorBasedMovementCosts() )
      {
    	  m_cAStar.udTerrainCost      = ASE_Costs::VectorBasedNormalMovementCosts;
        m_cAStar.udTerrainHeuristic = ASE_Costs::VectorBasedNormalMovementHeuristic;
      }
    else // cell based
      {
    	  m_cAStar.udTerrainCost      = ASE_Costs::CellBasedNormalMovementCosts;
        m_cAStar.udTerrainHeuristic = ASE_Costs::CellBasedNormalMovementHeuristic;
      }
  }


void CAseDoc::SelectThreatCostFunctions()
  {
    // no prediction of remaining threat costs
    const char* pStatusBarText;

    m_cAStar.udThreatHeuristic = ASE_Costs::ThreatsAnyModeAnyMovementHeuristic;
    pStatusBarText = "A* search: traditional shortest path";

    // select cost function as follows
    // - approximated lof table
    // - distance + count
    // - distance
    // - count
    // - no distance, no count
    // - none
    // and for each of these categories, distinguish aiming, vector/cell based

    if ( DoesUseThreatCostsFromApproxTable() )
       {
        if ( DoesUseAimingThreatCosts() )
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedThreatsApproximateLOFAimingCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedThreatsApproximateLOFAimingCosts;
              }
            pStatusBarText = "A* search: tactical, using costs from approximated small lookup table, taking into account aiming";
          } 
        else
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedThreatsApproximateLOFSimpleCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedThreatsApproximateLOFSimpleCosts;
              }
            pStatusBarText = "A* search: tactical, using costs from approximated small lookup table";
          } 
       }
    else
    if (   ( DoesUseDistanceToThreatCosts() )
        && ( DoesUseThreatCountCosts() )
       )
      {
        if ( DoesUseAimingThreatCosts() )
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedDistanceAndCountBasedAimingCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedDistanceAndCountBasedAimingCosts;
              }
            pStatusBarText = "A* search: tactical, using exact distance and threat count costs, taking into account aiming";
          }
        else
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedDistanceAndCountBasedSimpleCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedDistanceAndCountBasedSimpleCosts;
              }
            pStatusBarText = "A* search: tactical, using exact distance and threat count costs";
          }
      }
    else
    if ( DoesUseDistanceToThreatCosts() )
      {
        if ( DoesUseAimingThreatCosts() )
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedDistanceBasedAimingCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedDistanceBasedAimingCosts;
              }
            pStatusBarText = "A* search: tactical, using exact distance to threat costs, taking into account aiming";
          }
        else
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedDistanceBasedSimpleCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedDistanceBasedSimpleCosts;
              }
            pStatusBarText = "A* search: tactical, using exact distance to threat costs";
          }
      }
    else
    if ( DoesUseThreatCountCosts() )
      {
        if ( DoesUseAimingThreatCosts() )
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedCountBasedAimingCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedCountBasedAimingCosts;
              }
            pStatusBarText = "A* search: tactical, using exact threat count costs, taking into account aiming";
          }
        else
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedCountBasedSimpleCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedCountBasedSimpleCosts;
              }
            pStatusBarText = "A* search: tactical, using exact threat count costs";
          }
      }
    else
    if ( DoesUseThreatCosts() )
      {
        if ( DoesUseAimingThreatCosts() )
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedThreatsAimingCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedThreatsAimingCosts;
              }
            pStatusBarText = "A* search: tactical, using exact simple threat line-of-fire costs, taking into account aiming";
          }
        else
          {
            if ( DoesUseVectorBasedMovementCosts() )
              {
                m_cAStar.udThreatCost = ASE_Costs::VectorBasedThreatsSimpleCosts;
              }
            else
              {
                m_cAStar.udThreatCost = ASE_Costs::CellBasedThreatsSimpleCosts;
              }
            pStatusBarText = "A* search: tactical, using exact simple threat line-of-fire costs";
          }
      }
    else
      {
        m_cAStar.udThreatCost = ASE_Costs::ThreatsNoCosts;
      }

    SetStatusBarText(pStatusBarText);
  }


bool CAseDoc::SetupAStar(bool stepped/* = false*/)
{
	if (m_cStart.x == -1 || m_cEnd.x == -1) {
		MessageBox("Please ensure that both the start and end points are specified.\nA red and green square denote the start and end points, respectively.", "A* Explorer", MB_ICONERROR);
		return false;
	}

	CNodeView *nv = GetNodeView();
	GetAseView()->RemoveHighlight();

  assert( m_TerrainBoard.GetNumberOfRows() == ASE_BOARDX );
	m_cAStar.SetRows(ASE_BOARDX);
	m_cAStar.udValid = AS_Valid;

  ASE_Costs::SetTerrainBoard(GetTerrainBoard());
  ASE_Costs::SetThreatBoard(GetThreatBoard());
  ASE_Costs::SetLOFApproximationBoard(GetLOSApproximationBoard());

  SelectMovementCostFunctions();
  SelectThreatCostFunctions();

  if ( m_bThreatCostsFromApproximatedTable )
    {
      GetLOSApproximationBoard()->ComputeLinesOfFireApproximation();
    }

	m_cAStar.udNotifyList = CNodeView::OnUpdateList;
	m_cAStar.udNotifyChild = (m_bContinualUpdate) ? NULL : CNodeView::OnNotifyChild;

	m_cAStar.m_pCBData = reinterpret_cast<void *>(this);
	m_cAStar.m_pNCData = reinterpret_cast<void *>(nv);

	nv->OnPreAStar();

  m_SearchSpaceBoard.Clear();

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


void CAseDoc::OnMovementType(UINT uType)
  {
    m_bMovementDiagonal = ( uType == ID_MOVEMENT_DIAGONAL );
  }


void CAseDoc::OnUpdateUIMovementType(CCmdUI* pCmdUI)
  {
	  UINT uType = pCmdUI->m_nID;

	  pCmdUI->SetCheck(   (   ( uType == ID_MOVEMENT_MANHATTAN  )
                         && ( !m_bMovementDiagonal            )
                        )
                     || (   ( uType == ID_MOVEMENT_DIAGONAL   )
                         && ( m_bMovementDiagonal             )
                        )
                    );
  }

void CAseDoc::OnMovementCosts(UINT uCosts)
  {
    m_bMovementCostsVectorBased = ( uCosts == ID_MOVEMENTCOSTS_VECTOR );
    m_bMovementCostsRelative    = ( uCosts == ID_MOVEMENTCOSTS_RELATIVE );
  }


void CAseDoc::OnUpdateUIMovementCosts(CCmdUI* pCmdUI)
  {
	  UINT uCosts = pCmdUI->m_nID;

	  pCmdUI->SetCheck(   (   ( uCosts == ID_MOVEMENTCOSTS_VECTOR   )
                         && ( m_bMovementCostsVectorBased         )
                         && ( !m_bMovementCostsRelative           )
                        )
                     || (   ( uCosts == ID_MOVEMENTCOSTS_CELL     )
                         && ( !m_bMovementCostsVectorBased        )
                         && ( !m_bMovementCostsRelative           )
                        )
                     || (   ( uCosts == ID_MOVEMENTCOSTS_RELATIVE )
                         && ( m_bMovementCostsRelative            )
                        )
                    );
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
	if (m_bContinualUpdate && m_cStart.x != -1 && m_cEnd.x != -1) 
    {
		  m_bStepped = false;
		  OnExecuteAStar();
	  }
}

void CAseDoc::ListChanges(_asNode *pNode, UINT uChange)
{
  if ( uChange == 1 )
    {
      m_SearchSpaceBoard.MarkAsOpen(pNode->GetX(), pNode->GetY());
    }
  else
    {
      assert( uChange == 2 );
      m_SearchSpaceBoard.MarkAsClosed(pNode->GetX(), pNode->GetY());
    } 
}

void CAseDoc::OnViewSearchspace() 
{
	m_bSearchSpace = !m_bSearchSpace;

	UpdateAllViews(NULL);
}


void CAseDoc::OnUpdateViewSearchspace(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bSearchSpace);
}


void CAseDoc::OnViewLineOfFire() 
{
	m_bThreatLinesOfFire = !m_bThreatLinesOfFire;

	UpdateAllViews(NULL);
}


void CAseDoc::OnUpdateUIViewLineOfFire(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bThreatLinesOfFire);
}


void CAseDoc::OnThreatDistanceBasedCosts() 
{
	m_bDistanceToThreatCosts = !m_bDistanceToThreatCosts;

  SetThreatCostsMode();
  m_ThreatBoard.ComputeLinesOfFire();

	UpdateAllViews(NULL);
}


void CAseDoc::OnUpdateUIThreatDistanceBasedCosts(CCmdUI* pCmdUI) 
{
  if (   ( m_bThreatCostsFromApproximatedTable )
      || ( m_bThreatCostsNone )
     )
	  pCmdUI->Enable(false);
  else
    {
  	  pCmdUI->Enable();
  	  pCmdUI->SetCheck(m_bDistanceToThreatCosts);
    }
}


void CAseDoc::OnThreatCountBasedCosts() 
{
	m_bThreatCountCosts = !m_bThreatCountCosts;

  SetThreatCostsMode();
  m_ThreatBoard.ComputeLinesOfFire();

	UpdateAllViews(NULL);
}


void CAseDoc::OnUpdateUIThreatCountBasedCosts(CCmdUI* pCmdUI) 
{
  if (   ( m_bThreatCostsFromApproximatedTable )
      || ( m_bThreatCostsNone )
     )
	  pCmdUI->Enable(false);
  else
    {
  	  pCmdUI->Enable();
  	  pCmdUI->SetCheck(m_bThreatCountCosts);
    }
}


void CAseDoc::OnThreatFromApproxTableCosts() 
{
	m_bThreatCostsFromApproximatedTable = !m_bThreatCostsFromApproximatedTable;

  if ( m_bThreatCostsFromApproximatedTable )
    {
      m_bThreatCostsNone = false;

      if (   ( ! m_bThreatCostsSimple )
          && ( ! m_bThreatLineOfFireWithAimingCosts )
         )
        {
          m_bThreatCostsSimple = true;
        }
    }

  SetThreatCostsMode();

	UpdateAllViews(NULL);
}


void CAseDoc::OnUpdateUIThreatFromApproxTableCosts(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bThreatCostsFromApproximatedTable);
}


void CAseDoc::OnViewThreats() 
{
	m_bThreatPositions = !m_bThreatPositions;

	UpdateAllViews(NULL);
}


void CAseDoc::OnUpdateUIViewThreats(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bThreatPositions);
}


void CAseDoc::OnViewApproximatedLinesOfFire()
  {
    m_bDrawApproximatedLinesOfFire = !m_bDrawApproximatedLinesOfFire;

    if ( m_bDrawApproximatedLinesOfFire )
      {
        GetAseView()->DisplayWaitCursor();

        GetLOSApproximationBoard()->ComputeLinesOfFireApproximation();

        GetAseView()->DisplayDefaultCursor();
      }

    UpdateAllViews(NULL);
  }


void CAseDoc::OnUpdateUIViewApproximatedLinesOfFire(CCmdUI* pCmdUI)
  {
  	pCmdUI->SetCheck(m_bDrawApproximatedLinesOfFire);
  }


void CAseDoc::OnThreatCosts(UINT uCosts)
  {
    m_bThreatCostsSimple               = ( uCosts == ID_THREATCOSTS_LINEOFFIRE    );
    m_bThreatLineOfFireWithAimingCosts = ( uCosts == ID_THREATCOSTS_LOFWITHMEMORY );
    m_bThreatCostsNone                 = ( uCosts == ID_THREATCOSTS_NONE );

    if ( m_bThreatCostsNone )
      {
        m_bThreatCostsFromApproximatedTable = false;
      }
  }


void CAseDoc::OnUpdateUIThreatCosts(CCmdUI* pCmdUI)
  {
	  UINT uCosts = pCmdUI->m_nID;

	  pCmdUI->SetCheck(   (   ( uCosts == ID_THREATCOSTS_NONE )
                         && ( m_bThreatCostsNone            )
                        )
                     || (   ( uCosts == ID_THREATCOSTS_LINEOFFIRE   )
                         && ( m_bThreatCostsSimple                  )
                        )
                     || (   ( uCosts == ID_THREATCOSTS_LOFWITHMEMORY )
                         && ( m_bThreatLineOfFireWithAimingCosts     )
                        )
                    );
  }


void CAseDoc::OnThreatMovement(UINT uCosts)
  {
    unsigned int distance;
    distance = uCosts - ID_THREATMOVEMENT_0;
    assert( distance < 3 );
   
    m_ThreatBoard.SetAssumedThreatMovement(distance);
    m_ThreatBoard.ComputeLinesOfFire();

  	UpdateAllViews(NULL);
  }


void CAseDoc::OnUpdateUIThreatMovement(CCmdUI* pCmdUI)
  {
	  UINT uID = pCmdUI->m_nID;

	  pCmdUI->SetCheck(   (   ( uID == ID_THREATMOVEMENT_0 )
                         && ( 0 == m_ThreatBoard.GetAssumedThreatMovement() )
                        )
                     || (   ( uID == ID_THREATMOVEMENT_1 )
                         && ( 1 == m_ThreatBoard.GetAssumedThreatMovement() )
                        )
                     || (   ( uID == ID_THREATMOVEMENT_2 )
                         && ( 2 == m_ThreatBoard.GetAssumedThreatMovement() )
                        )
                    );

    // disable when using lookup table
    pCmdUI->Enable(!m_bThreatCostsFromApproximatedTable);
  }


void CAseDoc::SetThreatCostsMode()
  {
    if ( m_bThreatCountCosts && m_bDistanceToThreatCosts )
      {
        m_ThreatBoard.SetRiskModus(ASE_ThreatBoard::eMinLineOfFireCountAndDistance);
      } 
    else
    if ( m_bThreatCountCosts )
      {
        m_ThreatBoard.SetRiskModus(ASE_ThreatBoard::eMinLineOfFireCount);
      } 
    else
    if ( m_bDistanceToThreatCosts )
      {
        m_ThreatBoard.SetRiskModus(ASE_ThreatBoard::eMinLineOfFireDistance);
      } 
    else
      {
        m_ThreatBoard.SetRiskModus(ASE_ThreatBoard::eAnyLineOfFire);
      } 
  }
