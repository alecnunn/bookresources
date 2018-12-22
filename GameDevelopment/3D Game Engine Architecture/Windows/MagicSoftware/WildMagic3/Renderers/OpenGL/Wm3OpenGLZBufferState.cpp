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

GLenum OpenGLRenderer::ms_aeZBufferCompare[ZBufferState::CF_QUANTITY] =
{
    GL_NEVER,
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

//----------------------------------------------------------------------------
void OpenGLRenderer::SetZBufferState (ZBufferState* pkState)
{
    if ( pkState->Enabled )
    {
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(ms_aeZBufferCompare[pkState->Compare]);
    }
    else
    {
        glDisable(GL_DEPTH_TEST);
        glDepthFunc(GL_ALWAYS);
    }

    if ( pkState->Writable )
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);
}
//----------------------------------------------------------------------------
