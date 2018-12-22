// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PLANARREFLECTIONEFFECT_H
#define WM3PLANARREFLECTIONEFFECT_H

#include "Wm3Effect.h"
#include "Wm3TriMesh.h"

namespace Wm3
{

class WM3_ITEM PlanarReflectionEffect : public Effect
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    PlanarReflectionEffect (int iQuantity);
    virtual ~PlanarReflectionEffect ();

    // Create a clone of the effect.  The planes are shared.  The reflectances
    // are copied.  The stencil values are uniquely generated in the cloned
    // object.
    virtual Effect* Clone ();

    // member access
    int GetQuantity () const;
    void SetPlane (int i, TriMeshPtr spkPlane);
    TriMeshPtr GetPlane (int i) const;
    void SetReflectance (int i, float fReflectance);
    float GetReflectance (int i) const;

protected:
    PlanarReflectionEffect ();
    virtual void DoCloning (Effect* pkClone);

    int m_iQuantity;
    TriMeshPtr* m_aspkPlane;
    float* m_afReflectance;
};

WM3_REGISTER_STREAM(PlanarReflectionEffect);
typedef Pointer<PlanarReflectionEffect> PlanarReflectionEffectPtr;
#include "Wm3PlanarReflectionEffect.inl"

}

#endif
