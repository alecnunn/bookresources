// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3FresnelEffect.h"
#include "Wm3FresnelVShader.h"
#include "Wm3FresnelPShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,FresnelEffect,ShaderEffect);
WM3_IMPLEMENT_STREAM(FresnelEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(FresnelEffect,ShaderEffect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(FresnelEffect,ShaderEffect);

//----------------------------------------------------------------------------
FresnelEffect::FresnelEffect ()
{
    VShader = new FresnelVShader;
    PShader = new FresnelPShader;
}
//----------------------------------------------------------------------------
FresnelEffect::~FresnelEffect ()
{
}
//----------------------------------------------------------------------------
Effect* FresnelEffect::Clone ()
{
    FresnelEffect* pkClone = new FresnelEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void FresnelEffect::DoCloning (Effect* pkEffect)
{
    ShaderEffect::DoCloning(pkEffect);
}
//----------------------------------------------------------------------------
