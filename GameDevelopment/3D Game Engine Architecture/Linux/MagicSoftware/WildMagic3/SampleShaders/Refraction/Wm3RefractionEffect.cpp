// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3RefractionEffect.h"
#include "Wm3RefractionVShader.h"
#include "Wm3RefractionPShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,RefractionEffect,ShaderEffect);
WM3_IMPLEMENT_STREAM(RefractionEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(RefractionEffect,ShaderEffect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(RefractionEffect,ShaderEffect);

//----------------------------------------------------------------------------
RefractionEffect::RefractionEffect (Image* pkEnvMap0, Image* pkEnvMap1)
{
    VShader = new RefractionVShader;
    PShader = new RefractionPShader;

    Texture* pkEnvMap0Tex = new Texture;
    pkEnvMap0Tex->SetImage(pkEnvMap0);
    pkEnvMap0Tex->Mipmap = Texture::MM_NEAREST;
    pkEnvMap0Tex->Apply = Texture::AM_DECAL;
    Textures.Append(pkEnvMap0Tex);

    Texture* pkEnvMap1Tex = new Texture;
    pkEnvMap1Tex->SetImage(pkEnvMap1);
    pkEnvMap1Tex->Mipmap = Texture::MM_NEAREST;
    pkEnvMap1Tex->Apply = Texture::AM_DECAL;
    Textures.Append(pkEnvMap1Tex);
}
//----------------------------------------------------------------------------
RefractionEffect::RefractionEffect ()
{
}
//----------------------------------------------------------------------------
RefractionEffect::~RefractionEffect ()
{
}
//----------------------------------------------------------------------------
Effect* RefractionEffect::Clone ()
{
    RefractionEffect* pkClone = new RefractionEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void RefractionEffect::DoCloning (Effect* pkEffect)
{
    ShaderEffect::DoCloning(pkEffect);
}
//----------------------------------------------------------------------------
