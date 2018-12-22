// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3GlossMapEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,GlossMapEffect,Effect);
WM3_IMPLEMENT_STREAM(GlossMapEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(GlossMapEffect,Effect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(GlossMapEffect,Effect);

//----------------------------------------------------------------------------
GlossMapEffect::GlossMapEffect (Texture* pkTexture, Vector2fArray* pkUVs)
{
    Draw = &Renderer::DrawGlossMap;
    pkTexture->Apply = Texture::AM_MODULATE;
    Textures.Append(pkTexture);
    UVs.Append(pkUVs);
}
//----------------------------------------------------------------------------
GlossMapEffect::GlossMapEffect ()
{
    Draw = &Renderer::DrawGlossMap;
}
//----------------------------------------------------------------------------
GlossMapEffect::~GlossMapEffect ()
{
}
//----------------------------------------------------------------------------
Effect* GlossMapEffect::Clone ()
{
    GlossMapEffect* pkClone = new GlossMapEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void GlossMapEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);
}
//----------------------------------------------------------------------------
