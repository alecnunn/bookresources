// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3LightMapEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,LightMapEffect,Effect);
WM3_IMPLEMENT_STREAM(LightMapEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(LightMapEffect,Effect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(LightMapEffect,Effect);

//----------------------------------------------------------------------------
LightMapEffect::LightMapEffect (Texture* pkPrimaryTexture,
    Vector2fArray* pkPrimaryUVs, Texture* pkSecondaryTexture,
    Vector2fArray* pkSecondaryUVs, bool bHardAdd)
{
    pkPrimaryTexture->Apply = Texture::AM_REPLACE;
    Textures.Append(pkPrimaryTexture);
    UVs.Append(pkPrimaryUVs);

    if ( bHardAdd )
    {
        pkSecondaryTexture->Apply = Texture::AM_ADD;
    }
    else
    {
        pkSecondaryTexture->BlendColor = ColorRGBA::WHITE;
        pkSecondaryTexture->Apply = Texture::AM_COMBINE;
        pkSecondaryTexture->CombineFuncRGB = Texture::ACF_INTERPOLATE;
        pkSecondaryTexture->CombineSrc0RGB = Texture::ACS_CONSTANT;
        pkSecondaryTexture->CombineOp0RGB = Texture::ACO_SRC_COLOR;
        pkSecondaryTexture->CombineSrc1RGB = Texture::ACS_PREVIOUS;
        pkSecondaryTexture->CombineOp1RGB = Texture::ACO_SRC_COLOR;
        pkSecondaryTexture->CombineSrc2RGB = Texture::ACS_TEXTURE;
        pkSecondaryTexture->CombineOp2RGB = Texture::ACO_SRC_COLOR;
    }

    Textures.Append(pkSecondaryTexture);
    UVs.Append(pkSecondaryUVs);
}
//----------------------------------------------------------------------------
LightMapEffect::LightMapEffect ()
{
}
//----------------------------------------------------------------------------
LightMapEffect::~LightMapEffect ()
{
}
//----------------------------------------------------------------------------
Effect* LightMapEffect::Clone ()
{
    LightMapEffect* pkClone = new LightMapEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void LightMapEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);
}
//----------------------------------------------------------------------------
