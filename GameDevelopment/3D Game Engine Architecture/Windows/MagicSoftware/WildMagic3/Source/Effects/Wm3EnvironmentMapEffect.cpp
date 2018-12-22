// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3EnvironmentMapEffect.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,EnvironmentMapEffect,Effect);
WM3_IMPLEMENT_STREAM(EnvironmentMapEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(EnvironmentMapEffect,Effect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(EnvironmentMapEffect,Effect);

//----------------------------------------------------------------------------
EnvironmentMapEffect::EnvironmentMapEffect (Image* pkImage, int iApplyMode)
{
    Draw = &Renderer::DrawEnvironmentMap;

    Texture* pkTexture = new Texture;
    pkTexture->SetImage(pkImage);
    pkTexture->Apply = iApplyMode;
    pkTexture->Filter = Texture::FM_LINEAR;
    pkTexture->Mipmap = Texture::MM_NEAREST;
    pkTexture->Texgen = Texture::TG_ENVIRONMENT_MAP;
    Textures.Append(pkTexture);
    UVs.Append(0);
}
//----------------------------------------------------------------------------
EnvironmentMapEffect::EnvironmentMapEffect ()
{
    Draw = &Renderer::DrawEnvironmentMap;
}
//----------------------------------------------------------------------------
EnvironmentMapEffect::~EnvironmentMapEffect ()
{
}
//----------------------------------------------------------------------------
Effect* EnvironmentMapEffect::Clone ()
{
    EnvironmentMapEffect* pkClone = new EnvironmentMapEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void EnvironmentMapEffect::DoCloning (Effect* pkClone)
{
    Effect::DoCloning(pkClone);
}
//----------------------------------------------------------------------------
