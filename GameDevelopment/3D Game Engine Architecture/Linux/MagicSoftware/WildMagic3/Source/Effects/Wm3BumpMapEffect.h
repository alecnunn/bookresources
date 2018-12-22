// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BUMPMAPEFFECT_H
#define WM3BUMPMAPEFFECT_H

#include "Wm3Effect.h"
#include "Wm3AlphaState.h"
#include "Wm3ColorRGBArray.h"
#include "Wm3Light.h"

namespace Wm3
{

class WM3_ITEM BumpMapEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    BumpMapEffect (Image* pkPrimary, Vector2fArray* pkPrimaryUVs,
        Image* pkNormalMap, Vector2fArray* pkNormalUVs, Light* pkLight);

    virtual ~BumpMapEffect ();

    // Create a clone of the effect.  The light is shared.
    virtual Effect* Clone ();

    // member access
    Light* GetLight () const;

protected:
    BumpMapEffect ();
    virtual void DoCloning (Effect* pkClone);

    LightPtr m_spkLight;

// internal use
public:
    // Compute the light vectors whenever model space vertices, model space
    // normals, primary uvs, or light location/direction change.
    void ComputeLightVectors (Geometry* pkMesh);
};

WM3_REGISTER_STREAM(BumpMapEffect);
typedef Pointer<BumpMapEffect> BumpMapEffectPtr;
#include "Wm3BumpMapEffect.inl"

}

#endif
