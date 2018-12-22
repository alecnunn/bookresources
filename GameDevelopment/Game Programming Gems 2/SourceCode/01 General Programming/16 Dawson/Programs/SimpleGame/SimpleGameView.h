/* Copyright (C) Bruce Dawson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Bruce Dawson, 2001"
 */
// SimpleGameView.h : interface of the CSimpleGameView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SIMPLEGAMEVIEW_H__4D30C55E_0A7A_11D5_BA9B_000000000000__INCLUDED_)
#define AFX_SIMPLEGAMEVIEW_H__4D30C55E_0A7A_11D5_BA9B_000000000000__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CSimpleGameView : public CView
{
protected: // create from serialization only
	CSimpleGameView();
	DECLARE_DYNCREATE(CSimpleGameView)

// Attributes
public:
	CSimpleGameDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSimpleGameView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	afx_msg void OnDestroy();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSimpleGameView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CSimpleGameView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in SimpleGameView.cpp
inline CSimpleGameDoc* CSimpleGameView::GetDocument()
   { return (CSimpleGameDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIMPLEGAMEVIEW_H__4D30C55E_0A7A_11D5_BA9B_000000000000__INCLUDED_)
