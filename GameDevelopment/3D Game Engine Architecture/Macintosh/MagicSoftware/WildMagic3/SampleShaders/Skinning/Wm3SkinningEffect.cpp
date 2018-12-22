// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3SkinningEffect.h"
#include "Wm3SkinningVShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,SkinningEffect,ShaderEffect);
WM3_IMPLEMENT_STREAM(SkinningEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(SkinningEffect,ShaderEffect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(SkinningEffect,ShaderEffect);

//----------------------------------------------------------------------------
SkinningEffect::SkinningEffect (Vector2fArray* pkUVs, Vector2fArray* pkSTs, 
    ColorRGBArray* pkRGBs)
{
    VShader = new SkinningVShader;
    UVs.Append(pkUVs);
    UVs.Append(pkSTs);
    ColorRGBs = pkRGBs;
}
//----------------------------------------------------------------------------
SkinningEffect::SkinningEffect ()
{
}
//----------------------------------------------------------------------------
SkinningEffect::~SkinningEffect ()
{
}
//----------------------------------------------------------------------------
Effect* SkinningEffect::Clone ()
{
    SkinningEffect* pkClone = new SkinningEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void SkinningEffect::DoCloning (Effect* pkEffect)
{
    ShaderEffect::DoCloning(pkEffect);
}
//----------------------------------------------------------------------------
