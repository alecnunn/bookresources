// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SKINNINGEFFECT_H
#define WM3SKINNINGEFFECT_H

#include "Wm3ShaderEffect.h"
#include "Wm3SkinningVShader.h"

namespace Wm3
{

class WM3_ITEM SkinningEffect : public ShaderEffect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    SkinningEffect (Vector2fArray* pkUVs, Vector2fArray* pkSTs, 
        ColorRGBArray* pkRGBs);
    virtual ~SkinningEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

    void SetSkinningMat (int i, const Vector4f& rkData);
    Vector4f GetSkinningMat (int i) const;

protected:
    SkinningEffect ();

    // streaming
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(SkinningEffect);
typedef Pointer<SkinningEffect> SkinningEffectPtr;

#include "Wm3SkinningEffect.inl"

}

#endif
