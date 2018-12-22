// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3VertexShader.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,VertexShader,Shader);
WM3_IMPLEMENT_STREAM(VertexShader);
WM3_IMPLEMENT_DEFAULT_STREAM(VertexShader,Shader);
WM3_IMPLEMENT_DEFAULT_NAME_ID(VertexShader,Shader);

//----------------------------------------------------------------------------
VertexShader::VertexShader ()
    :
    Shader(ST_VERTEX_SHADER)
{
}
//----------------------------------------------------------------------------
VertexShader::~VertexShader ()
{
}
//----------------------------------------------------------------------------
