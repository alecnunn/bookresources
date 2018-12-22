#if !defined(AFX_FIXEDSPLITTER_H__AC888981_3EAA_11D6_9CD7_444553540000__INCLUDED_)
#define AFX_FIXEDSPLITTER_H__AC888981_3EAA_11D6_9CD7_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// fixedsplitter.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFixedSplitter frame with splitter

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CFixedSplitter : public CSplitterWnd
{
	DECLARE_DYNCREATE(CFixedSplitter)
public:
	CFixedSplitter();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixedSplitter)
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFixedSplitter();

	// Generated message map functions
	//{{AFX_MSG(CFixedSplitter)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FIXEDSPLITTER_H__AC888981_3EAA_11D6_9CD7_444553540000__INCLUDED_)
