// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3OpenGLRenderer.h"
using namespace Wm3;

//----------------------------------------------------------------------------
bool OpenGLRenderer::SelectFont (int iFontID)
{
    if ( 0 <= iFontID && iFontID < m_kDLInfo.GetQuantity() )
    {
        if ( m_kDLInfo[iFontID].Quantity > 0 )
        {
            m_iFontID = iFontID;
            return true;
        }
    }
    return false;
}
//----------------------------------------------------------------------------
void OpenGLRenderer::UnloadFont (int iFontID)
{
    // You may not unload the default font (id = 0).
    if ( 1 <= iFontID && iFontID < m_kDLInfo.GetQuantity() )
    {
        DisplayListInfo kInfo = m_kDLInfo[iFontID];
        glDeleteLists(kInfo.Start,kInfo.Quantity);
        m_kDLInfo.Remove(iFontID);
        if ( m_iFontID == iFontID )
            m_iFontID = 0;
    }
}
//----------------------------------------------------------------------------
void OpenGLRenderer::Draw (int iX, int iY, const ColorRGBA& rkColor,
    const char* acText)
{
    assert( acText );

    GLint iListBase;
    glGetIntegerv(GL_LIST_BASE,&iListBase);

    // switch to orthogonal view
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(-0.5f,m_iWidth-0.5f,-0.5f,m_iHeight-0.5f,-1.0f,1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // disable depth, lighting, and texturing
    GLboolean bDepthTest = glIsEnabled(GL_DEPTH_TEST);
    GLboolean bLighting = glIsEnabled(GL_LIGHTING);
    GLboolean bTexture2D = glIsEnabled(GL_TEXTURE_2D);
    if ( bDepthTest )
        glDisable(GL_DEPTH_TEST);
    if ( bLighting )
        glDisable(GL_LIGHTING);
    if ( bTexture2D )
        glDisable(GL_TEXTURE_2D);

    // set the text color
    glColor4fv((const float*)rkColor);

    // draw text string (use right-handed coordinates)
    glRasterPos3i(iX,m_iHeight-1-iY,0);
    glListBase(m_kDLInfo[m_iFontID].Base);
    glCallLists((GLsizei)strlen(acText),GL_UNSIGNED_BYTE,acText);

    // restore depth, lighting, and texturing
    if ( bDepthTest )
        glEnable(GL_DEPTH_TEST);
    if ( bLighting )
        glEnable(GL_LIGHTING);
    if ( bTexture2D )
        glEnable(GL_TEXTURE_2D);

    // restore matrices
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    // restore projective view
    OnFrustumChange();

    glListBase(iListBase);
}
//----------------------------------------------------------------------------
