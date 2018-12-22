// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3GLUTRENDERER_H
#define WM3GLUTRENDERER_H

#include "Wm3OpenGLRenderer.h"

namespace Wm3
{

class WM3_RENDERER_ITEM GlutRenderer : public OpenGLRenderer
{
public:
    // construction and destruction
    GlutRenderer (int iWindowID, int iWidth, int iHeight);

    virtual void ToggleFullscreen ();
    virtual void DisplayBackBuffer ();
    virtual void Draw (int iX, int iY, const ColorRGBA& rkColor,
        const char* acText);

    // TO DO. Alternate font handling has not yet been added to the GLUT
    // renderer.  For now the Load/Unload do nothing.
    virtual int LoadFont (const char* acFace, int iSize, bool bBold,
        bool bItalic);
    virtual void UnloadFont (int iFontID);

private:
    // GLUT identifier for the window of the renderer
    int m_iWindowID;

    // fullscreen/window support
    int m_iSaveWidth, m_iSaveHeight;
};

}

#endif
