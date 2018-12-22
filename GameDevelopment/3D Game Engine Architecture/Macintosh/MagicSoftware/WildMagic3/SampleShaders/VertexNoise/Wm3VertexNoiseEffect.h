// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3VERTEXNOISEEFFECT_H
#define WM3VERTEXNOISEEFFECT_H

#include "Wm3ShaderEffect.h"
#include "Wm3VertexNoiseVShader.h"

namespace Wm3
{

class WM3_ITEM VertexNoiseEffect : public ShaderEffect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    VertexNoiseEffect ();
    virtual ~VertexNoiseEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

    void SetNoiseTranslate (const Vector4f& rkNoiseTranslate);
    Vector4f GetNoiseTranslate () const;
    void SetNoiseScale (float fNoiseScale);
    float GetNoiseScale () const;
    void SetDisplacement (float fDisplacement);
    float GetDisplacement () const;
    void SetPg (int i, const Vector4f& rkData);
    Vector4f GetPg (int i) const;
    void SetBaseColor (const Vector4f& rkBaseColor);
    Vector4f GetBaseColor () const;

protected:
    // streaming
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(VertexNoiseEffect);
typedef Pointer<VertexNoiseEffect> VertexNoiseEffectPtr;

#include "Wm3VertexNoiseEffect.inl"

}

#endif
