
#ifndef _AFX_NODEVIEW_H_
#define _AFX_LEFTVIEW_H_

#if _MSC_VER > 1000
#pragma once
#endif

#include "pathfinder.h"

class CAseDoc;

class CNodeView : public CTreeView
{
protected:
	CNodeView();
	DECLARE_DYNCREATE(CNodeView)

public:
	CAseDoc* GetDocument();

public:
	//{{AFX_VIRTUAL(CNodeView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	//}}AFX_VIRTUAL

public:
	void DisplayGoalNode();
	void OnPostAStar(_asNode *);
	void OnPreAStar();
	void SortOpen();
	static int OnNotifyChild(_asNode *, _asNode *, int, void *);
	static int OnNotifyList(_asNode *, _asNode *, int, void *);
	virtual ~CNodeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CFont		m_cTahoma;
	CImageList	m_cImageList;

	HTREEITEM	m_hGoalNode;
	
	static TVSORTCB	m_tvSortData;
	static int CALLBACK SortByF(LPARAM, LPARAM, LPARAM);

	//{{AFX_MSG(CNodeView)
	afx_msg void OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LeftView.cpp
inline CAseDoc* CNodeView::GetDocument()
   { return (CAseDoc*)m_pDocument; }
#endif

//{{AFX_INSERT_LOCATION}}

#endif
