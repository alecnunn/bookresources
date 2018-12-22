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
void OpenGLRenderer::SetMaterialState (MaterialState* pkState)
{
    glMaterialfv(GL_FRONT,GL_EMISSION,(GLfloat*)pkState->Emissive);
    glMaterialfv(GL_FRONT,GL_AMBIENT,(GLfloat*)pkState->Ambient);
    glMaterialfv(GL_FRONT,GL_DIFFUSE,(GLfloat*)pkState->Diffuse);
    glMaterialfv(GL_FRONT,GL_SPECULAR,(GLfloat*)pkState->Specular);
    glMaterialf(GL_FRONT,GL_SHININESS,pkState->Shininess);
}
//----------------------------------------------------------------------------
