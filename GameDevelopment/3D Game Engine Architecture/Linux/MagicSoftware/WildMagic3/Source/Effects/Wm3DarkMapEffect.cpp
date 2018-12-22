// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DarkMapEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,DarkMapEffect,Effect);
WM3_IMPLEMENT_STREAM(DarkMapEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(DarkMapEffect,Effect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(DarkMapEffect,Effect);

//----------------------------------------------------------------------------
DarkMapEffect::DarkMapEffect (Texture* pkPrimaryTexture,
    Vector2fArray* pkPrimaryUVs, Texture* pkSecondaryTexture,
    Vector2fArray* pkSecondaryUVs)
{
    pkPrimaryTexture->Apply = Texture::AM_REPLACE;
    Textures.Append(pkPrimaryTexture);
    UVs.Append(pkPrimaryUVs);

    pkSecondaryTexture->Apply = Texture::AM_MODULATE;
    Textures.Append(pkSecondaryTexture);
    UVs.Append(pkSecondaryUVs);
}
//----------------------------------------------------------------------------
DarkMapEffect::DarkMapEffect ()
{
}
//----------------------------------------------------------------------------
DarkMapEffect::~DarkMapEffect ()
{
}
//----------------------------------------------------------------------------
Effect* DarkMapEffect::Clone ()
{
    DarkMapEffect* pkClone = new DarkMapEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void DarkMapEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);
}
//----------------------------------------------------------------------------
