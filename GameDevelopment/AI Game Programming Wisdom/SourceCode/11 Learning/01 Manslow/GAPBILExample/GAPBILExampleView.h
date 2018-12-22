//GAPBILExample
//Copyright John Manslow
//29/09/2001

// GAPBILExampleView.h : interface of the CGAPBILExampleView class
//
/////////////////////////////////////////////////////////////////////////////

class CGAPBILExampleView : public CView
{
protected: // create from serialization only
	CGAPBILExampleView();
	DECLARE_DYNCREATE(CGAPBILExampleView)

// Attributes
public:
	CGAPBILExampleDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGAPBILExampleView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGAPBILExampleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGAPBILExampleView)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GAPBILExampleView.cpp
inline CGAPBILExampleDoc* CGAPBILExampleView::GetDocument()
   { return (CGAPBILExampleDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
