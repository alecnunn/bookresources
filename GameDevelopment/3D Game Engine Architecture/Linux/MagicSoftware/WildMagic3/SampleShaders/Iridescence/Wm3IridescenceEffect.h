// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3IRIDESCENCEEFFECT_H
#define WM3IRIDESCENCEEFFECT_H

#include "Wm3ShaderEffect.h"
#include "Wm3IridescenceVShader.h"

namespace Wm3
{

class WM3_ITEM IridescenceEffect : public ShaderEffect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    IridescenceEffect (Image* pkRealTex, Image* pkGradientTex);
    virtual ~IridescenceEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

    void SetInterpolateFactor (float fInterpolateFactor);
    float GetInterpolateFactor () const;

protected:
    // streaming
    IridescenceEffect ();
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(IridescenceEffect);
typedef Pointer<IridescenceEffect> IridescenceEffectPtr;

#include "Wm3IridescenceEffect.inl"

}

#endif
