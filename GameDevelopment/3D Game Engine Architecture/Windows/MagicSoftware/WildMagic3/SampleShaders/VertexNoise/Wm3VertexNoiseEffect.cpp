// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3VertexNoiseEffect.h"
#include "Wm3VertexNoiseVShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,VertexNoiseEffect,ShaderEffect);
WM3_IMPLEMENT_STREAM(VertexNoiseEffect);
WM3_IMPLEMENT_DEFAULT_STREAM(VertexNoiseEffect,ShaderEffect);
WM3_IMPLEMENT_DEFAULT_NAME_ID(VertexNoiseEffect,ShaderEffect);

//----------------------------------------------------------------------------
VertexNoiseEffect::VertexNoiseEffect()
{
    VShader = new VertexNoiseVShader;
}
//----------------------------------------------------------------------------
VertexNoiseEffect::~VertexNoiseEffect ()
{
    // empty
}
//----------------------------------------------------------------------------
Effect* VertexNoiseEffect::Clone ()
{
    VertexNoiseEffect* pkClone = new VertexNoiseEffect;
    DoCloning(pkClone);
    return pkClone;
}
//----------------------------------------------------------------------------
void VertexNoiseEffect::DoCloning (Effect* pkEffect)
{
    ShaderEffect::DoCloning(pkEffect);
}
//----------------------------------------------------------------------------
