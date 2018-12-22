// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3WglRenderer.h"
using namespace Wm3;
using namespace std;

//----------------------------------------------------------------------------
WglRenderer::WglRenderer (HWND hWnd, int iWidth, int iHeight)
    :
    OpenGLRenderer(iWidth,iHeight)
{
    assert( m_iWidth > 0 && m_iHeight > 0 );
    m_iSaveWidth = m_iWidth;
    m_iSaveHeight = m_iHeight;

    m_hWnd = hWnd;
    m_hWindowDC = GetDC(m_hWnd);

    // select format for drawing surface
    PIXELFORMATDESCRIPTOR kPFD;
    memset(&kPFD,0,sizeof(PIXELFORMATDESCRIPTOR));
    kPFD.nSize = sizeof(PIXELFORMATDESCRIPTOR);
    kPFD.nVersion = 1;
    kPFD.dwFlags =
        PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL |
        PFD_GENERIC_ACCELERATED |
        PFD_DOUBLEBUFFER;
    kPFD.iPixelType = PFD_TYPE_RGBA;
    kPFD.cColorBits = 24;  // 24-bit colors for front/back buffers
    kPFD.cDepthBits = 16;  // 16-bit depth buffer
    kPFD.cStencilBits = 8; // 8-bit stencil buffer

    // set the pixel format for the rendering context
    int iPixelFormat = ChoosePixelFormat(m_hWindowDC,&kPFD);
    if ( iPixelFormat == 0 )
        return;

    BOOL bSuccess = SetPixelFormat(m_hWindowDC,iPixelFormat,&kPFD);
    if ( !bSuccess )
        return;

    // create an OpenGL context
    m_hWindowRC = wglCreateContext(m_hWindowDC);
    if ( !m_hWindowRC )
        return;

    bSuccess = wglMakeCurrent(m_hWindowDC,m_hWindowRC);
    if ( !bSuccess )
        return;

    GLenum uiError = glewInit();
    if ( uiError != GLEW_OK )
        return;

    // use the system font as default
    DisplayListInfo kInfo;
    int iFirstChar = int(' ');
    kInfo.Quantity = 127 - iFirstChar;
    kInfo.Start = glGenLists(kInfo.Quantity) ;
    kInfo.Base = kInfo.Start - iFirstChar;
    m_kDLInfo.Append(kInfo);
    m_hOldFont = SelectObject(m_hWindowDC,GetStockObject(SYSTEM_FONT)); 
    wglUseFontBitmaps(m_hWindowDC,iFirstChar,kInfo.Quantity,kInfo.Start);

    EstablishCapabilities();
    InitializeState();
}
//----------------------------------------------------------------------------
WglRenderer::~WglRenderer ()
{
    if ( m_hWindowRC )
        wglDeleteContext(m_hWindowRC);

    if ( m_hWindowDC )
    {
        SelectObject(m_hWindowDC,m_hOldFont);
        ReleaseDC(m_hWnd,m_hWindowDC);
    }
}
//----------------------------------------------------------------------------
void WglRenderer::ToggleFullscreen ()
{
    OpenGLRenderer::ToggleFullscreen();
    glFinish();

    DWORD dwStyle = WS_OVERLAPPEDWINDOW | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

    UINT uiFlags = SWP_NOACTIVATE | SWP_NOOWNERZORDER |
        SWP_NOSENDCHANGING | SWP_NOZORDER;

    RECT kRect;

    if ( m_bFullscreen )
    {
        m_iSaveWidth = m_iWidth;
        m_iSaveHeight = m_iHeight;
        GetWindowRect(GetDesktopWindow(),&kRect);
        AdjustWindowRect(&kRect,dwStyle,FALSE);
    }
    else
    {
        m_iWidth = m_iSaveWidth;
        m_iHeight = m_iSaveHeight;

        kRect.left = 0;
        kRect.top = 0;
        kRect.right = m_iWidth-1;
        kRect.bottom = m_iHeight-1;
        AdjustWindowRect(&kRect,dwStyle,FALSE);

        kRect.right -= kRect.left;
        kRect.left = 0;
        kRect.bottom -= kRect.top;
        kRect.top = 0;
    }

    SetWindowPos(m_hWnd,HWND_TOP,kRect.left,kRect.top,
        kRect.right-kRect.left,kRect.bottom-kRect.top,uiFlags);
}
//----------------------------------------------------------------------------
void WglRenderer::DisplayBackBuffer ()
{
    SwapBuffers(m_hWindowDC);
}
//----------------------------------------------------------------------------
int WglRenderer::LoadFont (const char* acFace, int iSize, bool bBold,
    bool bItalic)
{
    int iWeight = ( bBold ? FW_BOLD : FW_REGULAR );
        
    HFONT hFont = CreateFont(iSize,0,0,0,iWeight,(DWORD)bItalic,FALSE,
        FALSE,DEFAULT_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,
        ANTIALIASED_QUALITY,VARIABLE_PITCH,acFace);
    
    if ( !hFont )
        return -1;

    DisplayListInfo kInfo;
    int iFirstChar = int(' ');
    kInfo.Quantity = 127 - iFirstChar;
    kInfo.Start = glGenLists(kInfo.Quantity) ;
    kInfo.Base = kInfo.Start - iFirstChar;
    m_kDLInfo.Append(kInfo);

    SelectObject(m_hWindowDC,hFont); 
    wglUseFontBitmaps(m_hWindowDC,iFirstChar,kInfo.Quantity,kInfo.Start);
    DeleteObject(hFont);
    return m_kDLInfo.GetQuantity()-1;
}
//----------------------------------------------------------------------------
