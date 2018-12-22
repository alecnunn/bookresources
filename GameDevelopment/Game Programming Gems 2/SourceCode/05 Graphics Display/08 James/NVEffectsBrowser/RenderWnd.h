/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_RENDERWND_H__0823D2E9_F78A_11D4_8D9C_000347486A4B__INCLUDED_)
#define AFX_RENDERWND_H__0823D2E9_F78A_11D4_8D9C_000347486A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenderWnd window

class CRenderWnd : public CWnd
{
// Construction
public:
	CRenderWnd();
	DECLARE_DYNAMIC(CRenderWnd)

// Attributes
public:
	CNVEffectsDoc* GetDocument();
	CRenderView*   GetRenderView();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderWnd();

// Overrides
public:
	virtual HRESULT Init() = 0;
	virtual HRESULT Release() = 0;

	virtual HRESULT UpdateDevice() = 0;
	virtual HRESULT ConfirmDevice(EBEffect*) = 0;

	virtual HRESULT SetupEffect(EBEffect*) = 0;
	virtual HRESULT SetDefaultRenderstates() = 0;

	virtual HRESULT Render() = 0;
	virtual HRESULT Refresh(CDC*) = 0;
	virtual HRESULT Clear() = 0;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderWnd)
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERWND_H__0823D2E9_F78A_11D4_8D9C_000347486A4B__INCLUDED_)
