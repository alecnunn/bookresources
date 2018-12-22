// ChildView.h : interface of the ChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__A0C53901_5798_4823_82FA_0BF0156A87F6__INCLUDED_)
#define AFX_CHILDVIEW_H__A0C53901_5798_4823_82FA_0BF0156A87F6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// ChildView window

class ChildView : public CWnd
{
// Construction
public:
	ChildView();

	virtual ~ChildView();

protected:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(ChildView)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__A0C53901_5798_4823_82FA_0BF0156A87F6__INCLUDED_)
