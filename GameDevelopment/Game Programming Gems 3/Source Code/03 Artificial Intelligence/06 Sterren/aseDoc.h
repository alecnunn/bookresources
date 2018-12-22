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

#ifndef _AFX_ASEDOC_H_
#define _AFX_ASEDOC_H_

#if _MSC_VER > 1000
#pragma once
#endif

#define ASE_GRIDSIZE	8
#define ASE_BOARDX		320
#define ASE_BOARDY		240

#include "pathfinder.h"
#include "nodeview.h"
#include "aseview.h"

#include "ase_terrain.h"
#include "ase_threatboard.h"
#include "ase_search.h"
#include "ase_losapproxboard.h"


class CAseDoc : public CDocument
{
protected:
	CAseDoc();
	DECLARE_DYNCREATE(CAseDoc)

// movement type 
public:
  bool  DoesUseDiagonalMovement() const 
          { return ( m_bMovementDiagonal ); }

protected:
  bool    m_bMovementDiagonal;

// movement costs
public:
  bool  DoesUseCellBasedMovementCosts() const
           { return !m_bMovementCostsVectorBased; }
  bool  DoesUseVectorBasedMovementCosts() const
           { return m_bMovementCostsVectorBased; }
  bool  DoesUseRelativeMovementCosts() const
           { return m_bMovementCostsRelative; }
protected: // move to terrain?
  bool    m_bMovementCostsVectorBased;
	bool		m_bMovementCostsRelative;

  void    SelectMovementCostFunctions();

// threat costs
public:
  bool  DoesUseThreatCosts() const
           { return !m_bThreatCostsNone; }
  bool  DoesUseAimingThreatCosts() const
           { return m_bThreatLineOfFireWithAimingCosts; }
  bool  DoesUseThreatCountCosts() const
           { return m_bThreatCountCosts; }
  bool  DoesUseDistanceToThreatCosts() const
           { return m_bDistanceToThreatCosts; }
  bool  DoesUseThreatCostsFromApproxTable() const
           { return m_bThreatCostsFromApproximatedTable; }
protected: // move to threat board?
  bool    m_bThreatCostsNone;
  bool    m_bThreatCostsSimple;
  bool    m_bThreatLineOfFireWithAimingCosts;
  bool    m_bThreatCountCosts;
  bool    m_bDistanceToThreatCosts;
  bool    m_bThreatCostsFromApproximatedTable;

  void    SetThreatCostsMode();

  void    SelectThreatCostFunctions();

// views
public:
  bool    DoesDisplayThreatPositions() const
             { return m_bThreatPositions; }
  bool    DoesDisplayThreatLinesOfFire() const
             { return m_bThreatLinesOfFire; }
	bool	  DoesDisplaySearchSpace() const
             { return m_bSearchSpace; }
	bool	  DoesDisplayRoute() const
             { return m_bDrawRoute; }
	bool	  DoesDisplayApproximatedLinesOfFire() const
             { return m_bDrawApproximatedLinesOfFire; }
protected:
	bool		m_bSearchSpace;
  bool    m_bThreatPositions;
  bool    m_bThreatLinesOfFire;
	bool		m_bDrawRoute;
	bool		m_bDrawApproximatedLinesOfFire;

// a-star algorithm 
public:
	bool	Stepping() { return m_bStepped; }

	void	NodeAdded(_asNode *, int);
	void	NotifyClick();
	void	DrawNode(_asNode *);
	void	GetStartEnd(CPoint &x, CPoint &y) { x = m_cStart, y = m_cEnd; }
	void	SetStartEnd(CPoint x, CPoint y) { m_cStart = x, m_cEnd = y; }
	void	SetBreakpoint(CPoint bp) { m_cBreakpoint = bp; m_iBreakData = -1; }

	CPoint  GetBreakpoint() { return m_cBreakpoint; }
	CAStar *GetPathFinder() { return &m_cAStar; }

  //! check whether move is valid
	static int   AS_Valid(_asNode *, _asNode *, int, void *);

  ASE_TerrainBoard* GetTerrainBoard() 
    {
      return &m_TerrainBoard;
    }

  ASE_SearchBoard* GetSearchSpaceBoard()
    {
      return &m_SearchSpaceBoard;
    }

  ASE_ThreatBoard* GetThreatBoard()
    {
      return &m_ThreatBoard;
    }

  ASE_LOSApproximationBoard* GetLOSApproximationBoard()
    {
      return &m_LOSApproximationBoard;
    }

	//{{AFX_VIRTUAL(CAseDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

public:
	void ListChanges(_asNode *, UINT);
	virtual ~CAseDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
// status bar
  void    SetStatusBarText(const char* aText);

protected:
	bool		m_bStepped;
	bool		m_bBreakpointHit;
	bool		m_bContinualUpdate;

  //! boards used to describe/illustrate terrain, threat positions and search activity
  ASE_TerrainBoard          m_TerrainBoard;
  ASE_ThreatBoard           m_ThreatBoard;
  ASE_SearchBoard           m_SearchSpaceBoard;
  ASE_LOSApproximationBoard m_LOSApproximationBoard;

	int			m_iBreakData;
	
	CPoint		m_cStart;
	CPoint		m_cEnd;
	CPoint		m_cBreakpoint;
	UINT		m_uBrushType;

	_asNode		*m_pcBreakNode;

	CAStar		m_cAStar;

	bool		SetupAStar(bool stepped = false);
	
	inline void	MessageBox(CString, CString, UINT);

	CNodeView	*GetNodeView();
	CAseView	*GetAseView();
	
	//{{AFX_MSG(CAseDoc)
	afx_msg void OnRunToBreakpoint();
	afx_msg void OnUpdateRunToBreakpoint(CCmdUI* pCmdUI);
	afx_msg void OnExecuteAStar();
	afx_msg void OnStepAStar();
	afx_msg void OnUpdatePathingAllowDiagonal(CCmdUI* pCmdUI);
	afx_msg void OnViewARoute();
	afx_msg void OnUpdateViewARoute(CCmdUI* pCmdUI);
	afx_msg void OnPathingContinuousUpdate();
	afx_msg void OnUpdatePathingContinuousUpdate(CCmdUI* pCmdUI);
	afx_msg void OnViewSearchspace();
	afx_msg void OnUpdateViewSearchspace(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnBrushType(UINT);
	afx_msg void OnUpdateUIBrushType(CCmdUI* pCmdUI);
	afx_msg void OnBreakpointType(UINT);
	afx_msg void OnUpdateUIBreakpointType(CCmdUI* pCmdUI);

	afx_msg void OnViewLineOfFire();
	afx_msg void OnUpdateUIViewLineOfFire(CCmdUI* pCmdUI);
	afx_msg void OnViewThreats();
	afx_msg void OnUpdateUIViewThreats(CCmdUI* pCmdUI);
	afx_msg void OnViewApproximatedLinesOfFire();
	afx_msg void OnUpdateUIViewApproximatedLinesOfFire(CCmdUI* pCmdUI);

	afx_msg void OnMovementType(UINT);
	afx_msg void OnUpdateUIMovementType(CCmdUI* pCmdUI);
	afx_msg void OnMovementCosts(UINT);
	afx_msg void OnUpdateUIMovementCosts(CCmdUI* pCmdUI);

	afx_msg void OnThreatCosts(UINT);
	afx_msg void OnUpdateUIThreatCosts(CCmdUI* pCmdUI);

	afx_msg void OnThreatMovement(UINT);
	afx_msg void OnUpdateUIThreatMovement(CCmdUI* pCmdUI);

	afx_msg void OnThreatDistanceBasedCosts();
	afx_msg void OnUpdateUIThreatDistanceBasedCosts(CCmdUI* pCmdUI);

	afx_msg void OnThreatCountBasedCosts();
	afx_msg void OnUpdateUIThreatCountBasedCosts(CCmdUI* pCmdUI);

	afx_msg void OnThreatFromApproxTableCosts();
	afx_msg void OnUpdateUIThreatFromApproxTableCosts(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
