// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3REFRACTIONEFFECT_H
#define WM3REFRACTIONEFFECT_H

#include "Wm3ShaderEffect.h"
#include "Wm3RefractionVShader.h"

namespace Wm3
{

class WM3_ITEM RefractionEffect : public ShaderEffect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    RefractionEffect (Image* pkEnvMap0, Image* pkEnvMap1);
    virtual ~RefractionEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

    void SetIndexOfRefraction (float fIndexOfRefraction);
    float GetIndexOfRefraction () const;
    void SetFresnelConstants (const Vector4f& rkFresnelConstants);
    Vector4f GetFresnelConstants () const;

protected:
    // streaming
    RefractionEffect ();
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(RefractionEffect);
typedef Pointer<RefractionEffect> RefractionEffectPtr;

#include "Wm3RefractionEffect.inl"

}

#endif
