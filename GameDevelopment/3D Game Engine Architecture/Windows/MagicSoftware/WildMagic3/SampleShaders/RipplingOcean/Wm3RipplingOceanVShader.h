// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3RIPPLINGOCEANVSHADER_H
#define WM3RIPPLINGOCEANVSHADER_H

#include "Wm3VertexShader.h"

namespace Wm3
{

class WM3_ITEM RipplingOceanVShader : public VertexShader
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    RipplingOceanVShader ();
    virtual ~RipplingOceanVShader ();

    void SetConstants (const Vector4f& rkConstants);
    Vector4f GetConstants () const;
    void SetLightDir (const Vector3f& rkLightDir);
    Vector3f GetLightDir () const;
    void SetWaveDirX (const Vector4f& rkWaveDirX);
    Vector4f GetWaveDirX () const;
    void SetWaveDirY (const Vector4f& rkWaveDirY);
    Vector4f GetWaveDirY () const;
    void SetWaveSpeed (const Vector4f& rkWaveSpeed);
    Vector4f GetWaveSpeed () const;
    void SetWaveOffset (const Vector4f& rkWaveOffset);
    Vector4f GetWaveOffset () const;
    void SetWaveHeight (const Vector4f& rkWaveHeight);
    Vector4f GetWaveHeight () const;
    void SetBumpSpeed (const Vector4f& rkBumpSpeed);
    Vector4f GetBumpSpeed () const;

private:
    static const char* ms_aacProgram[Renderer::RT_QUANTITY];
};

WM3_REGISTER_STREAM(RipplingOceanVShader);
typedef Pointer<RipplingOceanVShader> RipplingOceanVShaderPtr;

#include "Wm3RipplingOceanVShader.inl"

}

#endif
