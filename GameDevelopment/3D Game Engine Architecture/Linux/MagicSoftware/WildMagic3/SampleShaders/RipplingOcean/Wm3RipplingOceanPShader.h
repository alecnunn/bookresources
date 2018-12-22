// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RIPPLINGOCEANPSHADER_H
#define WM3RIPPLINGOCEANPSHADER_H

#include "Wm3PixelShader.h"

namespace Wm3
{

class WM3_ITEM RipplingOceanPShader : public PixelShader
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    RipplingOceanPShader ();
    virtual ~RipplingOceanPShader ();


private:
    static const char* ms_aacProgram[Renderer::RT_QUANTITY];
};

WM3_REGISTER_STREAM(RipplingOceanPShader);
typedef Pointer<RipplingOceanPShader> RipplingOceanPShaderPtr;

}

#endif
