// ChildView.h : interface of the CChildView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHILDVIEW_H__6F71D25A_D83E_11D5_BA44_000102368FA3__INCLUDED_)
#define AFX_CHILDVIEW_H__6F71D25A_D83E_11D5_BA44_000102368FA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <math.h>
class VECTOR2D;

void DrawLine(VECTOR2D vOrig, VECTOR2D vLine, int iColorType = 0);
void DrawLine2(VECTOR2D vOrig, VECTOR2D vLine, int iColorType = 0);

/////////////////////////////////////////////////////////////////////////////
// CChildView window

class CChildView : public CWnd
{
// Construction
public:
	CChildView();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChildView)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChildView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CChildView)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown( UINT, CPoint );
	afx_msg void OnRButtonDown( UINT, CPoint );
	afx_msg void OnMouseMove( UINT nFlags, CPoint point );
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHILDVIEW_H__6F71D25A_D83E_11D5_BA44_000102368FA3__INCLUDED_)
