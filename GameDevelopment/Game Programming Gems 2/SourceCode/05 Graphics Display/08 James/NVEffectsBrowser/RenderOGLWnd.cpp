/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
// RenderOGLWnd.cpp : implementation file
//

#include "stdafx.h"

#include <GL/gl.h>

#include "nveffectsbrowser.h"
#include "RenderOGLWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRenderOGLWnd

IMPLEMENT_DYNCREATE(CRenderOGLWnd, CRenderWnd)

CRenderOGLWnd::CRenderOGLWnd()
  : m_bActive(false),
    m_pDC(NULL),
    m_hRC(0)
{
}

CRenderOGLWnd::~CRenderOGLWnd()
{

}


BEGIN_MESSAGE_MAP(CRenderOGLWnd, CWnd)
	//{{AFX_MSG_MAP(CRenderOGLWnd)
	ON_WM_SIZE()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_SYSKEYDOWN()
	ON_WM_SYSKEYUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRenderOGLWnd overridden methods

HRESULT CRenderOGLWnd::Init()
{
	return InitOpenGL();
}

HRESULT CRenderOGLWnd::Release()
{
	return ReleaseOpenGL();
}

HRESULT CRenderOGLWnd::UpdateDevice()
{
#if 0

	hr = ReleaseOpenGL();
	if (FAILED(hr)) return hr;

	hr = InitOpenGL();
	if (FAILED(hr)) return hr;

	return hr;

#else

	return S_OK;

#endif
}

HRESULT CRenderOGLWnd::ConfirmDevice(EBEffect *pEffect)
{
	return pEffect->ConfirmDevice(&m_sPFD);
}

HRESULT CRenderOGLWnd::SetupEffect(EBEffect *pEffect)
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;
	
	HRESULT hr;

	hr = ConfirmDevice(pEffect);
	if (FAILED(hr)) return hr;

	hr = SetDefaultRenderstates();
	if (FAILED(hr)) return hr;

	hr = pEffect->Initialize(m_hRC);
	if (FAILED(hr)) return hr;

	CRect r;
	GetClientRect(r);
	pEffect->Resize(GetSafeHwnd(), r.Width(), r.Height(), true);

	return S_OK;
}

HRESULT CRenderOGLWnd::SetDefaultRenderstates()
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;
	
	return S_OK;
}

HRESULT CRenderOGLWnd::Render()
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;

	CRect r;
	GetClientRect(&r);
	CNVEffectsDoc* pDoc = GetDocument();
	if (!IsWindowVisible() ||
		(r.Width() <= 0 || r.Height() <= 0) ||
		!pDoc->IsEffectValid())
		return NVEFF_ERR_NOTINITIALIZED;

	pDoc->Tick();

	if (!wglSwapLayerBuffers(m_pDC->GetSafeHdc(), WGL_SWAP_MAIN_PLANE))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderOGLWnd::Refresh(CDC*)
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;

	if (!wglSwapLayerBuffers(m_pDC->GetSafeHdc(), WGL_SWAP_MAIN_PLANE))
		return E_FAIL;

	return S_OK;
}

HRESULT CRenderOGLWnd::Clear()
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;

	float grey = ((float) 0xAA) / ((float) 0xFF);
	glClearColor(grey, grey, grey, 0.0);

	if (!wglSwapLayerBuffers(m_pDC->GetSafeHdc(), WGL_SWAP_MAIN_PLANE))
		return E_FAIL;

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRenderOGLWnd message handlers

void CRenderOGLWnd::OnSize(UINT nType, int cx, int cy) 
{
	CRenderWnd::OnSize(nType, cx, cy);
}

void CRenderOGLWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRenderOGLWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CRenderOGLWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnLButtonDown(nFlags, point);
}

void CRenderOGLWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnLButtonUp(nFlags, point);
}

void CRenderOGLWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnMouseMove(nFlags, point);
}

void CRenderOGLWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnRButtonDown(nFlags, point);
}

void CRenderOGLWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnRButtonUp(nFlags, point);
}

void CRenderOGLWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CRenderOGLWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// TODO: Add your message handler code here and/or call default
	
	CRenderWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CRenderOGLWnd private methods

HRESULT CRenderOGLWnd::InitOpenGL()
{
	m_pDC = new CClientDC(this);
	if (m_pDC == NULL) return E_FAIL;

	if (!SetupPixelFormat())
		return E_FAIL;

	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (m_hRC == 0) return E_FAIL;

	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return E_FAIL;
	
	m_bActive = true;

	return S_OK;
}

HRESULT CRenderOGLWnd::ReleaseOpenGL()
{
	m_bActive = false;

    // delete the RC
    if (m_hRC)
		if (wglDeleteContext(m_hRC) == false)
				return E_FAIL;
	m_hRC = NULL;
	
    // delete the DC
    SAFE_DELETE(m_pDC);

	return S_OK;
}

BOOL CRenderOGLWnd::SetupPixelFormat(PIXELFORMATDESCRIPTOR* pPFD)
{
    // Default pixel format for a double-buffered,
    // OpenGL-supporting, hardware-accelerated, 
    // RGBA-mode format. Pass in a pointer to a different
    // pixel format if you want something else
    PIXELFORMATDESCRIPTOR pfd = 
	{
        sizeof(PIXELFORMATDESCRIPTOR),// size of this pfd
			1,                      // version number
			PFD_DRAW_TO_WINDOW |    // support window
			PFD_DOUBLEBUFFER |      // support double buffering
			PFD_SUPPORT_OPENGL,     // support OpenGL
			PFD_TYPE_RGBA,          // RGBA type
			32,                     // color depth
			0, 0, 0, 0, 0, 0,       // color bits/shifts
			0, 0,                   // alpha bits/shifts
			0,                      // accum bits
			0, 0, 0, 0,             // r/g/b/a accum bits
			16,                     // depth bits
			0,                      // stencil bits
			0,                      // auxiliary buffers
			PFD_MAIN_PLANE,         // main layer
			0, 0, 0, 0              // masks
	};

    PIXELFORMATDESCRIPTOR* pPFDtoUse;

    // let the user override the default pixel format
    pPFDtoUse = (0 == pPFD)? &pfd : pPFD; 
 
    int pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
    if (pixelformat == 0) return false;

    if (!SetPixelFormat( m_pDC->GetSafeHdc(), pixelformat, pPFDtoUse))
		return false;

	// Now squirrell away the pixelformat
	if (!DescribePixelFormat(m_pDC->GetSafeHdc(), pixelformat, sizeof(m_sPFD), &m_sPFD))
		return false;

    return TRUE;
}
