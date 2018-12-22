// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PLANARSHADOWEFFECT_H
#define WM3PLANARSHADOWEFFECT_H

#include "Wm3Effect.h"
#include "Wm3Light.h"
#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM PlanarShadowEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    PlanarShadowEffect (int iQuantity);
    virtual ~PlanarShadowEffect ();

    // Create a clone of the effect.  The planes and projectors are shared.
    // The shadow colors are copied.  The stencil values are uniquely
    // generated in the cloned object.
    virtual Effect* Clone ();

    // member access
    int GetQuantity () const;
    void SetPlane (int i, TriMeshPtr spkPlane);
    TriMeshPtr GetPlane (int i) const;
    void SetProjector (int i, LightPtr spkProjector);
    LightPtr GetProjector (int i) const;
    void SetShadowColor (int i, const ColorRGBA& rkShadowColor);
    const ColorRGBA& GetShadowColor (int i) const;

protected:
    PlanarShadowEffect ();
    virtual void DoCloning (Effect* pkClone);

    int m_iQuantity;
    TriMeshPtr* m_aspkPlane;
    LightPtr* m_aspkProjector;
    ColorRGBA* m_akShadowColor;
};

WM3_REGISTER_STREAM(PlanarShadowEffect);
typedef Pointer<PlanarShadowEffect> PlanarShadowEffectPtr;
#include "Wm3PlanarShadowEffect.inl"

}

#endif
