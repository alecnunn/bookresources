// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3GlutRenderer.h"
using namespace Wm3;

//----------------------------------------------------------------------------
GlutRenderer::GlutRenderer (int iWindowID, int iWidth, int iHeight)
    :
    OpenGLRenderer(iWidth,iHeight)
{
    assert( m_iWidth > 0 && m_iHeight > 0 );
    m_iSaveWidth = m_iWidth;
    m_iSaveHeight = m_iHeight;

    m_iWindowID = iWindowID;
    m_iWidth = iWidth;
    m_iHeight = iHeight;

    GLenum uiError = glewInit();
    if ( uiError != GLEW_OK )
        return;

    EstablishCapabilities();
    InitializeState();
}
//----------------------------------------------------------------------------
void GlutRenderer::ToggleFullscreen ()
{
    OpenGLRenderer::ToggleFullscreen();

    if ( m_bFullscreen )
    {
        m_iSaveWidth = m_iWidth;
        m_iSaveHeight = m_iHeight;
        glutFullScreen();
    }
    else
    {
        glutPositionWindow(24,24);
        glutReshapeWindow(m_iSaveWidth,m_iSaveHeight);
        Resize(m_iSaveWidth,m_iSaveHeight);
    }
}
//----------------------------------------------------------------------------
void GlutRenderer::DisplayBackBuffer ()
{
    glutSwapBuffers();
}
//----------------------------------------------------------------------------
void GlutRenderer::Draw (int iX, int iY, const ColorRGBA& rkColor,
    const char* acText)
{
    assert( acText );

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
    int iLength = (int)strlen(acText);
    for (int i = 0; i < iLength; i++)
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13,acText[i]);

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
}
//----------------------------------------------------------------------------
int GlutRenderer::LoadFont (const char*, int, bool, bool)
{
    // TO DO.
    return -1;
}
//----------------------------------------------------------------------------
void GlutRenderer::UnloadFont (int)
{
    // TO DO.
}
//----------------------------------------------------------------------------
