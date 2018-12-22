/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_RENDERVIEW_H__0823D2E5_F78A_11D4_8D9C_000347486A4B__INCLUDED_)
#define AFX_RENDERVIEW_H__0823D2E5_F78A_11D4_8D9C_000347486A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderView.h : header file
//

class CNVEffectsDoc;
class CRenderWnd;
class CRenderD3DWnd;
class CRenderOGLWnd;

/////////////////////////////////////////////////////////////////////////////
// CRenderView view

class CRenderView : public CView
{
protected:
	CRenderView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CRenderView)

// Attributes
public:
	CNVEffectsDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderView)
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CRenderView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	enum RenderMode {
		NoRenderMode  = 0,
		D3DRenderMode = 1,
		OGLRenderMode = 2
	};

	HRESULT SetRenderMode(RenderMode mode);

	void UpdateDevice(EBEffect*);
	void SetMsg(const CString &str, BOOL fForceRedraw = false);

	CRenderWnd*    GetCurWnd()       { ASSERT(m_pCurWnd); return m_pCurWnd; };
	CRenderD3DWnd* GetRenderD3DWnd() { return m_pRenderD3DWnd; };
    CRenderOGLWnd* GetRenderOGLWnd() { return m_pRenderOGLWnd; };

protected:
	bool              m_fResizing;
	RenderMode        m_eRenderMode;
	CString           m_strMsg;
	CRenderWnd*		  m_pCurWnd;
    CRenderD3DWnd*    m_pRenderD3DWnd;
    CRenderOGLWnd*    m_pRenderOGLWnd;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERVIEW_H__0823D2E5_F78A_11D4_8D9C_000347486A4B__INCLUDED_)
