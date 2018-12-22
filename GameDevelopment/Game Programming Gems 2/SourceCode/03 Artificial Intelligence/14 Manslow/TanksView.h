//						Tanks
//			(c) Copyright John Manslow
//					  18/04/2001

// TanksView.h : interface of the CTanksView class
//
/////////////////////////////////////////////////////////////////////////////

class CTanksView : public CView
{
protected: // create from serialization only
	CTanksView();
	DECLARE_DYNCREATE(CTanksView)

// Attributes
public:
	CTanksDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTanksView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTanksView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTanksView)
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in TanksView.cpp
inline CTanksDoc* CTanksView::GetDocument()
   { return (CTanksDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
