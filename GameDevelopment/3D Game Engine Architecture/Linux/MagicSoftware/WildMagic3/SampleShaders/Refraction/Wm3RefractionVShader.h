// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3REFRACTIONVSHADER_H
#define WM3REFRACTIONVSHADER_H

#include "Wm3VertexShader.h"

namespace Wm3
{

class WM3_ITEM RefractionVShader : public VertexShader
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    RefractionVShader ();
    virtual ~RefractionVShader ();

    void SetIndexOfRefraction (float fIndexOfRefraction);
    float GetIndexOfRefraction () const;
    void SetFresnelConstants (const Vector4f& rkFresnelConstants);
    Vector4f GetFresnelConstants () const;

private:
    static const char* ms_aacProgram[Renderer::RT_QUANTITY];
};

WM3_REGISTER_STREAM(RefractionVShader);
typedef Pointer<RefractionVShader> RefractionVShaderPtr;

#include "Wm3RefractionVShader.inl"

}

#endif
