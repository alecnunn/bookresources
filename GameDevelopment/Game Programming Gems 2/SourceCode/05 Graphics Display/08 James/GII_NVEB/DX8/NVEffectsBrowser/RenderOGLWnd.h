/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_RENDEROGLWND_H__05A50DC0_F92B_11D4_8D9C_000347486A4B__INCLUDED_)
#define AFX_RENDEROGLWND_H__05A50DC0_F92B_11D4_8D9C_000347486A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderOGLWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRenderOGLWnd window

class CRenderOGLWnd : public CRenderWnd
{
// Construction
public:
	CRenderOGLWnd();
	DECLARE_DYNCREATE(CRenderOGLWnd)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderOGLWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderOGLWnd();

// Overrides
public:
	virtual HRESULT Init();
	virtual HRESULT Release();

	virtual HRESULT UpdateDevice();
	virtual HRESULT ConfirmDevice(EBEffect*);

	virtual HRESULT SetupEffect(EBEffect*);
	virtual HRESULT SetDefaultRenderstates();

	virtual HRESULT Render(bool bDrawLogo = false);
	virtual HRESULT Refresh(CDC*);
	virtual HRESULT Clear();

protected:
    HRESULT RecoverSwapFailure();

	HRESULT InitOpenGL();
	HRESULT ReleaseOpenGL();

	HRESULT GetBitmapBits(UINT nID, UINT *pW, UINT *pH, BYTE **lpPixels);

	HRESULT InitLogo();
	HRESULT ReleaseLogo();
	HRESULT DrawLogo();

	HRESULT SetupPixelFormat(PIXELFORMATDESCRIPTOR *pFD=NULL);

	BOOL						m_bActive, m_bClean;
	CClientDC                  *m_pDC;
	HGLRC                       m_hRC, m_hLogoRC;
	int                         m_iPixelFormat;
	PIXELFORMATDESCRIPTOR       m_sPFD;

	BOOL                        m_bLogoValid;
	UINT                        m_nLogoWidth, m_nLogoHeight, m_nLogoWidth2, m_nLogoHeight2;
	UINT                        m_nLogoTexID;

	UINT                        m_nDispListID;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderOGLWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDEROGLWND_H__05A50DC0_F92B_11D4_8D9C_000347486A4B__INCLUDED_)
