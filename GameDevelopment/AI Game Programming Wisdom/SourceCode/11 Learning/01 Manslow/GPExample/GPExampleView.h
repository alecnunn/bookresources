// GPExampleView.h : interface of the CGPExampleView class
//
/////////////////////////////////////////////////////////////////////////////

class CGPExampleView : public CView
{
protected: // create from serialization only
	CGPExampleView();
	DECLARE_DYNCREATE(CGPExampleView)

// Attributes
public:
	CGPExampleDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGPExampleView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CGPExampleView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CGPExampleView)
	afx_msg void OnTimer(UINT nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in GPExampleView.cpp
inline CGPExampleDoc* CGPExampleView::GetDocument()
   { return (CGPExampleDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
