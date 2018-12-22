// RTTIPropsView.h : interface of the CRTTIPropsView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RTTIPROPSVIEW_H__636565A4_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_)
#define AFX_RTTIPROPSVIEW_H__636565A4_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CRTTIPropsView : public CView
{
protected: // create from serialization only
	CRTTIPropsView();
	DECLARE_DYNCREATE(CRTTIPropsView)

// Attributes
public:
	CRTTIPropsDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRTTIPropsView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRTTIPropsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CRTTIPropsView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RTTIPropsView.cpp
inline CRTTIPropsDoc* CRTTIPropsView::GetDocument()
   { return (CRTTIPropsDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTTIPROPSVIEW_H__636565A4_8F89_11D7_9CE2_E807A7DC092F__INCLUDED_)
