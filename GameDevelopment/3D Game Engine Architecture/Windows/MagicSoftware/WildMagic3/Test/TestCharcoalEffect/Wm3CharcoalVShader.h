// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CHARCOALVSHADER_H
#define WM3CHARCOALVSHADER_H

#include "Wm3VertexShader.h"

namespace Wm3
{

class WM3_ITEM CharcoalVShader : public VertexShader
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    CharcoalVShader ();
    virtual ~CharcoalVShader ();

    void SetAmbientIntensity (float fAmbientIntensity);
    float GetAmbientIntensity () const;
    void SetContrastExponent (float fContrastExponent);
    float GetContrastExponent () const;

private:
    static const char* ms_aacProgram[Renderer::RT_QUANTITY];
};

WM3_REGISTER_STREAM(CharcoalVShader);
typedef Pointer<CharcoalVShader> CharcoalVShaderPtr;

#include "Wm3CharcoalVShader.inl"

}

#endif
