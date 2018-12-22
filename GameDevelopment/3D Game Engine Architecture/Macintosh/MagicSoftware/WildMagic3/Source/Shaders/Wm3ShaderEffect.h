// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SHADEREFFECT_H
#define WM3SHADEREFFECT_H

#include "Wm3Effect.h"
#include "Wm3VertexShader.h"
#include "Wm3PixelShader.h"

namespace Wm3
{

class WM3_ITEM ShaderEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    ShaderEffect ();
    virtual ~ShaderEffect ();

    // Create a clone of the effect.  Shaders are shared.
    virtual Effect* Clone ();

    VertexShaderPtr VShader;
    PixelShaderPtr PShader;

protected:
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(ShaderEffect);
typedef Pointer<ShaderEffect> ShaderEffectPtr;

}

#endif
