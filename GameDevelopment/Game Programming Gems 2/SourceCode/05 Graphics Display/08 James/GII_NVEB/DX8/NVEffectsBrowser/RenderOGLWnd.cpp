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

#include <GL/gl.h>
#include <GL/glu.h>

#include "StdAfx.h"
#include "NVEffectsBrowser.h"
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
    m_bClean(false),
	m_iPixelFormat(-1),
    m_bLogoValid(false),
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
	return S_OK;
}

HRESULT CRenderOGLWnd::ConfirmDevice(EBEffect *pEffect)
{
	HRESULT hr = pEffect->ConfirmDevice(&m_sPFD);
    if (FAILED(hr))
        SetDefaultError(_T("This effect cannot run in this window or on this hardware."));

    return hr;
}

HRESULT CRenderOGLWnd::SetupEffect(EBEffect *pEffect)
{
	HRESULT hr;

	hr = SetDefaultRenderstates();
	if (FAILED(hr))
        return hr;

	hr = ConfirmDevice(pEffect);
	if (FAILED(hr))
        return hr;

	// Mark the context as dirty (must be replaced to reset state)
	m_bClean  = false;

	hr = pEffect->Initialize(m_hRC);
    if (FAILED(hr)) {
        SetDefaultError(_T("This effect was unable to initialize."));
        return hr;
    }

	CRect r;
	GetClientRect(r);
	pEffect->Resize(GetSafeHwnd(), r.Width(), r.Height(), true);

	return S_OK;
}

HRESULT CRenderOGLWnd::SetDefaultRenderstates()
{
	if (m_bClean)
		return S_OK;

	// Make no context current
	if (!wglMakeCurrent(NULL, NULL))
		return E_FAIL;

    // Delete the effect's RC
    if (m_hRC)
		if (wglDeleteContext(m_hRC) == false)
				return E_FAIL;
	m_hRC = NULL;
	
     
    SAFE_DELETE(m_pDC);

    m_pDC = new CClientDC(this);
    if (m_pDC == 0)
        return E_FAIL;

 	// Get a rendering context for the effect
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (m_hRC == 0)
        return E_FAIL;

	// Make the effect context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return E_FAIL;

	// Mark the context as clean
	m_bClean  = true;

    return S_OK;
}

HRESULT CRenderOGLWnd::Render(bool bDrawLogo)
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;

	// Short circuit if the window is hidden or somesuch...
	CRect r;
	GetClientRect(&r);
	CNVEffectsDoc* pDoc = GetDocument();
	if (!IsWindowVisible() ||
		(r.Width() <= 0 || r.Height() <= 0) ||
		!pDoc->IsEffectValid())
		return NVEFF_ERR_NOTINITIALIZED;

	// Call the effect
    HRESULT hr = pDoc->Tick();
    if (FAILED(hr))
        return hr;

	// Possibly draw the burned-in logo
	if (bDrawLogo && m_bLogoValid)
		DrawLogo();

	// Display the frame
	if (!SwapBuffers(m_pDC->GetSafeHdc()))
    {
        hr = RecoverSwapFailure();
        if (FAILED(hr))
            return hr;
    }
	// Update the FPS, TPS, VPS labels...
	GetRenderView()->UpdateLabels();

	return S_OK;
}

HRESULT CRenderOGLWnd::RecoverSwapFailure()
{
	if (!wglMakeCurrent(NULL, NULL))
		return E_FAIL;

    SAFE_DELETE(m_pDC);

    m_pDC = new CClientDC(this);
    if (m_pDC == 0)
        return E_FAIL;

	// Make the effect context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return E_FAIL;

    return S_OK;
}

HRESULT CRenderOGLWnd::Refresh(CDC*)
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;

	if (!SwapBuffers(m_pDC->GetSafeHdc()))
    {
        HRESULT hr = RecoverSwapFailure();
        if (FAILED(hr))
            return hr;
    }

	return S_OK;
}

HRESULT CRenderOGLWnd::Clear()
{
	ASSERT(m_pDC);
	if (!m_pDC)
		return NVEFF_ERR_NOTINITIALIZED;

    // Save the clear color
    glPushAttrib(GL_COLOR_BUFFER_BIT);

    // Clear the window to the system color
    COLORREF c = GetSysColor(COLOR_3DFACE);
    float r = ((c & 0x0000ff) >>  0) / 255.0f;
    float g = ((c & 0x00ff00) >>  8) / 255.0f;
    float b = ((c & 0xff0000) >> 16) / 255.0f;

    glClearColor(r, g, b, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

    // Restore the clear color
    glPopAttrib();

	if (!SwapBuffers(m_pDC->GetSafeHdc()))
    {
        HRESULT hr = RecoverSwapFailure();
        if (FAILED(hr))
            return hr;
    }

	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CRenderOGLWnd message handlers

void CRenderOGLWnd::OnSize(UINT nType, int cx, int cy) 
{
	CRenderWnd::OnSize(nType, cx, cy);

	// Short circuit if this window is hidden
	if (!IsWindowVisible())
		return;
}

void CRenderOGLWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CRenderOGLWnd::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CRenderOGLWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnLButtonDown(nFlags, point);
}

void CRenderOGLWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnLButtonUp(nFlags, point);
}

void CRenderOGLWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnMouseMove(nFlags, point);
}

void CRenderOGLWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnRButtonDown(nFlags, point);
}

void CRenderOGLWnd::OnRButtonUp(UINT nFlags, CPoint point) 
{
	CRenderWnd::OnRButtonUp(nFlags, point);
}

void CRenderOGLWnd::OnSysKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnSysKeyDown(nChar, nRepCnt, nFlags);
}

void CRenderOGLWnd::OnSysKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CRenderWnd::OnSysKeyUp(nChar, nRepCnt, nFlags);
}

/////////////////////////////////////////////////////////////////////////////
// CRenderOGLWnd private methods

HRESULT CRenderOGLWnd::InitOpenGL()
{
	if (m_bActive)
		return S_OK;

	HRESULT hr;

	// Get the DC
	m_pDC = new CClientDC(this);
	if (m_pDC == NULL) return E_FAIL;

	// Find an appropriate pixel format
	hr = SetupPixelFormat();
	if (FAILED(hr))
		return hr;

	// Get a rendering context for the logo
	m_hLogoRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (m_hLogoRC == 0) return E_FAIL;

    // Make the logo context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hLogoRC))
		return E_FAIL;

	// Init the logo
	m_bLogoValid = false;
	hr = InitLogo();
	if (FAILED(hr))
		return hr;

 	// Get a rendering context for the effect
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (m_hRC == 0) return E_FAIL;

	// Make the effect context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return E_FAIL;

	m_bClean  = true;
	m_bActive = true;
	return S_OK;
}

HRESULT CRenderOGLWnd::ReleaseOpenGL()
{
	if (!m_bActive)
		return S_OK;

    // Make the logo context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hLogoRC))
		return E_FAIL;

	if (m_bLogoValid)
		ReleaseLogo();

	// Make no context current
	if (!wglMakeCurrent(NULL, NULL))
		return E_FAIL;

    // Delete the logo RCs
    if (m_hLogoRC)
		if (wglDeleteContext(m_hLogoRC) == false)
				return E_FAIL;
	m_hLogoRC = NULL;

    // Delete the effect RCs
    if (m_hRC)
		if (wglDeleteContext(m_hRC) == false)
				return E_FAIL;
	m_hRC = NULL;

    // Delete the DC
    SAFE_DELETE(m_pDC);

	m_bLogoValid = false;
	m_bClean  = false;
	m_bActive = false;
	return S_OK;
}

static int ChooseAcceleratedPixelFormat( HDC hdc, CONST PIXELFORMATDESCRIPTOR *  ppfd )
{
    int pixelformat = ChoosePixelFormat(hdc, ppfd);

    if ( pixelformat == 0 ) return pixelformat;

    PIXELFORMATDESCRIPTOR pfd;
    if (!DescribePixelFormat(hdc, pixelformat, sizeof(pfd), &pfd)) return 0;

    int accel = ((pfd.dwFlags)&PFD_GENERIC_FORMAT) > 0 ? 0 : 1;
    if ( !accel ) return 0;

    return pixelformat;
}

HRESULT CRenderOGLWnd::SetupPixelFormat(PIXELFORMATDESCRIPTOR* pPFD)
{
	// Only do this once...
	if (m_iPixelFormat != -1)
		return S_OK;

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
			8, 0,                   // alpha bits/shifts
			0,                      // accum bits
			0, 0, 0, 0,             // r/g/b/a accum bits
			24,                     // depth bits
			8,                      // stencil bits
			0,                      // auxiliary buffers
			PFD_MAIN_PLANE,         // main layer
			0, 0, 0, 0              // masks
	};

    PIXELFORMATDESCRIPTOR* pPFDtoUse;

    // let the user override the default pixel format
    pPFDtoUse = (0 == pPFD)? &pfd : pPFD; 
 
    int pixelformat;

    // First try to get a 24b depth buffer
    pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);

    if (pixelformat == 0) {
        
        // Now try a 16b depth buffer
        pPFDtoUse->cDepthBits = 16;
        pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
        if (pixelformat == 0) {
        
            // Now try a 24b depth buffer without stencil
            pPFDtoUse->cDepthBits = 24;
            pPFDtoUse->cStencilBits = 0;
            pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
            if (pixelformat == 0) {
                
                // Now try a 16b depth buffer without stencil
                pPFDtoUse->cDepthBits = 16;
                pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
                if (pixelformat == 0) {

                    // Try 16b color formats.
                    pPFDtoUse->cColorBits = 16;
                    pPFDtoUse->cAlphaBits = 0;
                    pPFDtoUse->cDepthBits = 24;
                    pPFDtoUse->cStencilBits = 8;

                    // First try to get a 24b depth buffer
                    pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
                    if (pixelformat == 0) {
        
                        // Now try a 16b depth buffer
                        pPFDtoUse->cDepthBits = 16;
                        pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
                        if (pixelformat == 0) {
        
                            // Now try a 24b depth buffer without stencil
                            pPFDtoUse->cDepthBits = 24;
                            pPFDtoUse->cStencilBits = 0;
                            pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
                            if (pixelformat == 0) {
                
                                // Now try a 16b depth buffer without stencil
                                pPFDtoUse->cDepthBits = 16;
                                pixelformat = ChooseAcceleratedPixelFormat(m_pDC->GetSafeHdc(), pPFDtoUse);
                                if (pixelformat == 0) {

                                    // Ok, no good buffer, fail
                                    if (pixelformat == 0)
                                        return false;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    if (!SetPixelFormat( m_pDC->GetSafeHdc(), pixelformat, pPFDtoUse))
		return E_FAIL;

	// Now squirrel away the pixelformat
	m_iPixelFormat = pixelformat;
	if (!DescribePixelFormat(m_pDC->GetSafeHdc(), pixelformat, sizeof(m_sPFD), &m_sPFD))
		return E_FAIL;

    return S_OK;
}

HRESULT CRenderOGLWnd::InitLogo()
{
	if (m_bLogoValid)
		return S_OK;

	m_bLogoValid = false;

	// Get the logo bits
	HRESULT hr;
	BYTE *lpImg;
	UINT w, h, w2, h2;

	hr = GetRenderView()->GetSharedLogoBits(&lpImg, &w, &h, &w2, &h2);
	if (FAILED(hr) || !lpImg)
		return E_FAIL;

	m_nLogoWidth   = w;
	m_nLogoWidth2  = w2;
	m_nLogoHeight  = h;
	m_nLogoHeight2 = h2;

	// Get the logo location
	float tx, ty, sx, sy, cx, cy;
	UINT winw, winh;
	GetRenderView()->GetSharedLogoPosition(&tx, &ty, &sx, &sy, &cx, &cy, &winw, &winh);

	// Make the logo context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hLogoRC))
		return E_FAIL;

	// Setup the transformation matrices
	glViewport(0, 0, winw, winh); 

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glMatrixMode(GL_TEXTURE);
	glLoadIdentity();

	// Setup the state
	glAlphaFunc(GL_GREATER, 0.5f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glEnable(GL_TEXTURE_2D);

	// Create the OpenGL texture
	glGenTextures(1, &m_nLogoTexID);
	glBindTexture(GL_TEXTURE_2D, m_nLogoTexID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 4, w2, h2, 0, GL_BGRA_EXT, GL_UNSIGNED_BYTE, lpImg);

	// Create the display list
	m_nDispListID = glGenLists(1);
	glNewList(m_nDispListID, GL_COMPILE);
	{
		glBegin(GL_QUADS);
		{
			float tc[] = {  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,  0.0f,  1.0f };
			float vp[] = { -1.0f, -1.0f,  1.0f, -1.0f,  1.0f,  1.0f, -1.0f,  1.0f };
			
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			for (int i=0; i<4; i++) {
				glTexCoord2f(tx*tc[2*i+0], ty*tc[2*i+1]);
				glVertex3f(sx*vp[2*i+0]+cx, sy*vp[2*i+1]+cy, -1.0f);
			}
		}
		glEnd();
	}
	glEndList();

	// Check for errors with our setup...
	GLenum err = glGetError();
	ASSERT(err == GL_NO_ERROR);

	m_bLogoValid = true;
	return S_OK;
}


HRESULT CRenderOGLWnd::ReleaseLogo()
{
	if (!m_bLogoValid)
		return S_OK;

	glDeleteLists(m_nDispListID, 1);
	glDeleteTextures(1, &m_nLogoTexID);

	m_bLogoValid = false;
	return S_OK;
}


HRESULT CRenderOGLWnd::DrawLogo()
{
	if (!m_bLogoValid)
		return E_FAIL;

	// Make the logo context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hLogoRC))
		return E_FAIL;

	// Draw the logo
	glCallList(m_nDispListID);
	
	// Check for errors with our logo rendering...
	GLenum err = glGetError();
	ASSERT(err == GL_NO_ERROR);

	// Make the effect context current
	if (!wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC))
		return E_FAIL;

	return S_OK;
}
