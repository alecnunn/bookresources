// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3FRESNELPSHADER_H
#define WM3FRESNELPSHADER_H

#include "Wm3PixelShader.h"

namespace Wm3
{

class WM3_ITEM FresnelPShader : public PixelShader
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    FresnelPShader ();
    virtual ~FresnelPShader ();


private:
    static const char* ms_aacProgram[Renderer::RT_QUANTITY];
};

WM3_REGISTER_STREAM(FresnelPShader);
typedef Pointer<FresnelPShader> FresnelPShaderPtr;

}

#endif
