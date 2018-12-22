
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

class CAseDoc : public CDocument
{
protected:
	CAseDoc();
	DECLARE_DYNCREATE(CAseDoc)

public:
	bool	SetDiagonal(int);
	bool	Stepping() { return m_bStepped; }
	bool	GetDiagonal() { return m_bAllowDiagonal; }
	bool	DrawRoute() { return m_bDrawRoute; }
	char	*GetBoard() { return &(m_cBoard[0][0]); }

	void	NodeAdded(_asNode *, int);
	void	NotifyClick();
	void	DrawNode(_asNode *);
	void	GetStartEnd(CPoint &x, CPoint &y) { x = m_cStart, y = m_cEnd; }
	void	SetStartEnd(CPoint x, CPoint y) { m_cStart = x, m_cEnd = y; }
	void	SetBreakpoint(CPoint bp) { m_cBreakpoint = bp; m_iBreakData = -1; }

	CPoint  GetBreakpoint() { return m_cBreakpoint; }
	CAStar *GetPathFinder() { return &m_cAStar; }

	static int AS_Valid(_asNode *, _asNode *, int, void *);
	static int AS_Cost(_asNode *, _asNode *, int, void *);
	static int AS_RelativeCost(_asNode *, _asNode *, int, void *);

	//{{AFX_VIRTUAL(CAseDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

public:
	virtual ~CAseDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	bool		m_bStepped;
	bool		m_bBreakpointHit;
	bool		m_bDrawRoute;
	bool		m_bContinualUpdate;
	bool		m_bAllowDiagonal;	
	bool		m_bRelativeCosting;
	char		m_cBoard[ASE_BOARDX][ASE_BOARDY];
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
	afx_msg void OnPathingAllowDiagonal();
	afx_msg void OnUpdatePathingAllowDiagonal(CCmdUI* pCmdUI);
	afx_msg void OnViewARoute();
	afx_msg void OnUpdateViewARoute(CCmdUI* pCmdUI);
	afx_msg void OnPathingContinuousUpdate();
	afx_msg void OnUpdatePathingContinuousUpdate(CCmdUI* pCmdUI);
	afx_msg void OnPathingRelativeCosting();
	afx_msg void OnUpdatePathingRelativeCosting(CCmdUI* pCmdUI);
	//}}AFX_MSG
	afx_msg void OnBrushType(UINT);
	afx_msg void OnUpdateUIBrushType(CCmdUI *);
	afx_msg void OnBreakpointType(UINT);
	afx_msg void OnUpdateUIBreakpointType(CCmdUI *);
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}

#endif
