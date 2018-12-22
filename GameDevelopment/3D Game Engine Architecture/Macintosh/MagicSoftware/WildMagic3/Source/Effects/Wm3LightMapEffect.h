// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3LIGHTMAPEFFECT_H
#define WM3LIGHTMAPEFFECT_H

#include "Wm3Effect.h"

namespace Wm3
{

class WM3_ITEM LightMapEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    LightMapEffect (Texture* pkPrimaryTexture, Vector2fArray* pkPrimaryUVs,
        Texture* pkSecondaryTexture, Vector2fArray* pkSecondaryUVs,
        bool bHardAdd = true);
    virtual ~LightMapEffect ();

    // create a clone of the effect
    virtual Effect* Clone ();

protected:
    LightMapEffect ();
    virtual void DoCloning (Effect* pkClone);
};

WM3_REGISTER_STREAM(LightMapEffect);
typedef Pointer<LightMapEffect> LightMapEffectPtr;

}

#endif
