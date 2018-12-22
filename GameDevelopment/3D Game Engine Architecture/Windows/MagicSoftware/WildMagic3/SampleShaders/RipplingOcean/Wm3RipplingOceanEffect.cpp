// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3RipplingOceanEffect.h"
#include "Wm3RipplingOceanVShader.h"
#include "Wm3RipplingOceanPShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,RipplingOceanEffect,ShaderEffect);
WM3_IMPLEMENT_STREAM(RipplingOceanEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(RipplingOceanEffect,ShaderEffect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(RipplingOceanEffect,ShaderEffect);

//----------------------------------------------------------------------------
RipplingOceanEffect::RipplingOceanEffect (Image* pkBump, Image* pkWater, 
    Image* pkEnv, Vector2fArray* pkUVs, ColorRGBArray* pkRGBs)
{
    VShader = new RipplingOceanVShader;
    PShader = new RipplingOceanPShader;

    Texture* pkBumpTex = new Texture;
    pkBumpTex->SetImage(pkBump);
    pkBumpTex->Mipmap = Texture::MM_LINEAR_LINEAR;
    pkBumpTex->Filter = Texture::FM_LINEAR;
    pkBumpTex->Apply = Texture::AM_DECAL;
    pkBumpTex->CoordU = Texture::WM_REPEAT;
    pkBumpTex->CoordV = Texture::WM_REPEAT;
    Textures.Append(pkBumpTex);

    Texture* pkWaterTex = new Texture;
    pkWaterTex->SetImage(pkWater);
    pkWaterTex->Apply = Texture::AM_DECAL;
    pkWaterTex->CoordU = Texture::WM_CLAMP;
    pkWaterTex->CoordV = Texture::WM_CLAMP;
    Textures.Append(pkWaterTex);

    Texture* pkEnvTex = new Texture;
    pkEnvTex->SetImage(pkEnv);
    pkEnvTex->Mipmap = Texture::MM_NEAREST;
    pkEnvTex->Apply = Texture::AM_DECAL;
    Textures.Append(pkEnvTex);

    UVs.Append(pkUVs);
    ColorRGBs = pkRGBs;
}
//----------------------------------------------------------------------------
RipplingOceanEffect::RipplingOceanEffect ()
{
}
//----------------------------------------------------------------------------
RipplingOceanEffect::~RipplingOceanEffect ()
{
}
//----------------------------------------------------------------------------
Effect* RipplingOceanEffect::Clone ()
{
    RipplingOceanEffect* pkClone = new RipplingOceanEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void RipplingOceanEffect::DoCloning (Effect* pkEffect)
{
    ShaderEffect::DoCloning(pkEffect);
}
//----------------------------------------------------------------------------
