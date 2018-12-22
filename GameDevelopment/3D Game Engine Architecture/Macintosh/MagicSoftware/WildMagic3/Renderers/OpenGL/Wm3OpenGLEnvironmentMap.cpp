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
#include "Wm3Node.h"
#include "Wm3EnvironmentMapEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void OpenGLRenderer::DrawEnvironmentMap ()
{
    // Access the special effect.  Detach it from the node to allow the
    // effectless node drawing.
    assert( DynamicCast<EnvironmentMapEffect>(m_pkGlobalEffect) );
    EnvironmentMapEffectPtr spkEMEffect =
        (EnvironmentMapEffect*)m_pkGlobalEffect;
    m_pkNode->SetEffect(0);

    // Draw the Node tree.  Any Geometry objects with textures will have the
    // environment map as an additional one, drawn after the others
    // according to the apply mode stored by the environment map.
    m_pkNode->Draw(*this);

    // reattach the effect
    m_pkNode->SetEffect(spkEMEffect);
}
//----------------------------------------------------------------------------
