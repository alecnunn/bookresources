// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DxRenderer.h"
#include "Wm3Node.h"
#include "Wm3ProjectedTextureEffect.h"
using namespace Wm3;

//----------------------------------------------------------------------------
void DxRenderer::DrawProjectedTexture ()
{
    // Access the special effect.  Detach it from the node to allow the
    // effectless node drawing.
    assert( DynamicCast<ProjectedTextureEffect>(m_pkGlobalEffect) );
    ProjectedTextureEffectPtr spkPTEffect =
        (ProjectedTextureEffect*)m_pkGlobalEffect;
    m_pkNode->SetEffect(0);

    // Draw the Node tree.  Any Geometry objects with textures will have the
    // projected texture as an additional one, drawn after the others
    // according to the apply mode stored by the projected texture.
    m_pkNode->Draw(*this);

    // reattach the effect
    m_pkNode->SetEffect(spkPTEffect);
}
//----------------------------------------------------------------------------
