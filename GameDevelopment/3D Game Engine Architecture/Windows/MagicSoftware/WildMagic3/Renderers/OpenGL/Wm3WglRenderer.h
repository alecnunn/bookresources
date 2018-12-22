// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3WGLRENDERER_H
#define WM3WGLRENDERER_H

#include "Wm3OpenGLRenderer.h"

namespace Wm3
{

class WM3_RENDERER_ITEM WglRenderer : public OpenGLRenderer
{
public:
    // construction and destruction
    WglRenderer (HWND hWnd, int iWidth, int iHeight);
    virtual ~WglRenderer ();

    virtual void ToggleFullscreen ();
    virtual void DisplayBackBuffer ();
    virtual int LoadFont (const char* acFace, int iSize, bool bBold,
        bool bItalic);

protected:
    // window parameters
    HWND m_hWnd;
    HDC m_hWindowDC;
    HGLRC m_hWindowRC;
    HGDIOBJ m_hOldFont;

    // fullscreen/window support
    int m_iSaveWidth, m_iSaveHeight;
};

}

#endif
