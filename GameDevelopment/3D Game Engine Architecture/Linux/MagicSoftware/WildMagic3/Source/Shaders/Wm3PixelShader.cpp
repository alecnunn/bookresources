// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3PixelShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,PixelShader,Shader);
WM3_IMPLEMENT_STREAM(PixelShader);
WM3_IMPLEMENT_DEFAULT_STREAM(PixelShader,Shader);
WM3_IMPLEMENT_DEFAULT_NAME_ID(PixelShader,Shader);

//----------------------------------------------------------------------------
PixelShader::PixelShader ()
    :
    Shader(ST_PIXEL_SHADER)
{
}
//----------------------------------------------------------------------------
PixelShader::~PixelShader ()
{
}
//----------------------------------------------------------------------------
