// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IridescenceEffect.h"
#include "Wm3IridescenceVShader.h"
#include "Wm3IridescencePShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,IridescenceEffect,ShaderEffect);
WM3_IMPLEMENT_STREAM(IridescenceEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(IridescenceEffect,ShaderEffect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(IridescenceEffect,ShaderEffect);

//----------------------------------------------------------------------------
IridescenceEffect::IridescenceEffect (Image* pkRealTex, Image* pkGradientTex)
{
    VShader = new IridescenceVShader;
    PShader = new IridescencePShader;

    Texture* pkRealTexTex = new Texture;
    pkRealTexTex->SetImage(pkRealTex);
    pkRealTexTex->Mipmap = Texture::MM_NEAREST;
    pkRealTexTex->Apply = Texture::AM_DECAL;
    Textures.Append(pkRealTexTex);

    Texture* pkGradientTexTex = new Texture;
    pkGradientTexTex->SetImage(pkGradientTex);
    pkGradientTexTex->Mipmap = Texture::MM_NEAREST;
    pkGradientTexTex->Apply = Texture::AM_DECAL;
    Textures.Append(pkGradientTexTex);
}
//----------------------------------------------------------------------------
IridescenceEffect::IridescenceEffect ()
{
}
//----------------------------------------------------------------------------
IridescenceEffect::~IridescenceEffect ()
{
}
//----------------------------------------------------------------------------
Effect* IridescenceEffect::Clone ()
{
    IridescenceEffect* pkClone = new IridescenceEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void IridescenceEffect::DoCloning (Effect* pkEffect)
{
    ShaderEffect::DoCloning(pkEffect);
}
//----------------------------------------------------------------------------
