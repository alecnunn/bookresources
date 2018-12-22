/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#if !defined(AFX_RENDERD3DWND_H__0823D2E6_F78A_11D4_8D9C_000347486A4B__INCLUDED_)
#define AFX_RENDERD3DWND_H__0823D2E6_F78A_11D4_8D9C_000347486A4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RenderD3DWnd.h : header file
//

#include "RenderWnd.h"

/////////////////////////////////////////////////////////////////////////////
// CRenderD3DWnd window

//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------
#define MAX_FONT_VERTICES 50*6

struct FONT2DVERTEX { D3DXVECTOR4 p;   DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DXVECTOR3 p;   D3DXVECTOR3 n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

static inline FONT2DVERTEX InitFont2DVertex( const D3DXVECTOR4& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

static inline FONT3DVERTEX InitFont3DVertex( const D3DXVECTOR3& p, const D3DXVECTOR3& n,
                                      FLOAT tu, FLOAT tv )
{
    FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
    return v;
}

// Font creation flags
#define D3DFONT_BOLD        0x0001
#define D3DFONT_ITALIC      0x0002

// Font rendering flags
#define D3DFONT_CENTERED    0x0001
#define D3DFONT_TWOSIDED    0x0002
#define D3DFONT_FILTERED    0x0004

class CRenderD3DWnd : public CRenderWnd
{
// Construction
public:
	CRenderD3DWnd();
	DECLARE_DYNCREATE(CRenderD3DWnd)

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenderD3DWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRenderD3DWnd();

// Overrides
public:
	virtual HRESULT Init();
	virtual HRESULT Release();

	virtual void    GetDevice(LPDIRECT3DDEVICE8*);
	virtual HRESULT UpdateDevice();
	virtual HRESULT ConfirmDevice(EBEffect*);

	virtual HRESULT SetupEffect(EBEffect*);
	virtual HRESULT SetDefaultRenderstates();

	virtual HRESULT Render();
	virtual HRESULT Refresh(CDC*);
	virtual HRESULT Clear();

#if 0
	void Activate(bool bActivate)
	{
		m_bActive = bActivate;
		if (!bActivate)
			RedrawWindow(NULL, NULL, RDW_FRAME);
	}
#endif

private:
	HRESULT InitD3D();
	HRESULT ReleaseD3D();
	HRESULT SetDefaultRenderstatesD3D();

	// Logo rendering stuff
	HRESULT ResizeLogo();
	HRESULT DrawLogo();

	// Framerate rendering stuff
	HRESULT SetupTextImage();
	HRESULT SetupTextBuffer();
	HRESULT FreeTextBuffer();
	HRESULT DrawText( FLOAT sx, FLOAT sy, DWORD dwColor, TCHAR* strText, DWORD dwFlags = 0);


	void InterpretError(HRESULT hr);

	// Global state
	D3DDEVTYPE      m_devType;
	DWORD           m_devBhv;
	D3DFORMAT       m_colorBufferFormat;
	D3DMULTISAMPLE_TYPE m_MultiSampleType;
	D3DPRESENT_PARAMETERS m_d3dpp;

	LPDIRECT3DDEVICE8 m_pD3DDev;
	bool m_bSizing;
	bool m_bActive;
	bool m_bValid;

	LPDIRECT3DTEXTURE8 m_pLogoTexture;
    LPDIRECT3DVERTEXBUFFER8 m_pLogoVB;

    TCHAR   m_strFontName[80];            // Font properties
    DWORD   m_dwFontHeight;
    DWORD   m_dwFontFlags;
    LPDIRECT3DTEXTURE8 m_pFontTexture;   // The d3d texture for this font
    LPDIRECT3DVERTEXBUFFER8 m_pFontVB;        // VertexBuffer for rendering text
    DWORD   m_dwFontTexWidth;						// Texture dimensions
    DWORD   m_dwFontTexHeight;
    FLOAT   m_fFontTextScale;
    FLOAT   m_fFontTexCoords[128-32][4];

	bool m_bUseSWVP;

	// Generated message map functions
protected:
	//{{AFX_MSG(CRenderD3DWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RENDERD3DWND_H__0823D2E6_F78A_11D4_8D9C_000347486A4B__INCLUDED_)
