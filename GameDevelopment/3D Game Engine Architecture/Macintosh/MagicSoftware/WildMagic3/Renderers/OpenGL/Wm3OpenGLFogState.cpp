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

GLenum OpenGLRenderer::ms_aeFogDensity[FogState::DF_QUANTITY] =
{
    GL_LINEAR,
    GL_EXP,
    GL_EXP2
};

GLenum OpenGLRenderer::ms_aeFogApply[FogState::AF_QUANTITY] =
{
    GL_FASTEST,
    GL_NICEST
};

//----------------------------------------------------------------------------
void OpenGLRenderer::SetFogState (FogState* pkState)
{
    if ( pkState->Enabled )
    {
        glEnable(GL_FOG);
        glFogf(GL_FOG_START,pkState->Start);
        glFogf(GL_FOG_END,pkState->End);
        glFogfv(GL_FOG_COLOR,(GLfloat*)pkState->Color);
        glFogf(GL_FOG_DENSITY,pkState->Density);
        glFogi(GL_FOG_MODE,ms_aeFogDensity[pkState->DensityFunction]);
        glHint(GL_FOG_HINT,ms_aeFogApply[pkState->ApplyFunction]);
    }
    else
    {
        glDisable(GL_FOG);
    }
}
//----------------------------------------------------------------------------
