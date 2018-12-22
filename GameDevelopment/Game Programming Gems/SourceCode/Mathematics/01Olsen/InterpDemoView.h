/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
// InterpDemoView.h : interface of the CInterpDemoView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_INTERPDEMOVIEW_H__04D7808C_FEA4_11D3_9A3F_004F49077001__INCLUDED_)
#define AFX_INTERPDEMOVIEW_H__04D7808C_FEA4_11D3_9A3F_004F49077001__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CInterpDemoView : public CView
{
protected: // create from serialization only
	CInterpDemoView();
	DECLARE_DYNCREATE(CInterpDemoView)

// Attributes
public:
	CInterpDemoDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInterpDemoView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInterpDemoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CInterpDemoView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in InterpDemoView.cpp
inline CInterpDemoDoc* CInterpDemoView::GetDocument()
   { return (CInterpDemoDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INTERPDEMOVIEW_H__04D7808C_FEA4_11D3_9A3F_004F49077001__INCLUDED_)
