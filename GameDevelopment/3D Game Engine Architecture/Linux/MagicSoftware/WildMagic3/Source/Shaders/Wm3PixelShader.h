// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PIXELSHADER_H
#define WM3PIXELSHADER_H

#include "Wm3Shader.h"

namespace Wm3
{

class WM3_ITEM PixelShader : public Shader
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    PixelShader ();
    virtual ~PixelShader ();
};

WM3_REGISTER_STREAM(PixelShader);
typedef Pointer<PixelShader> PixelShaderPtr;

}

#endif
