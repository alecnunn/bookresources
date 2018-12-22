// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRRAY3SPHERE3_H
#define WM3INTRRAY3SPHERE3_H

#include "Wm3Intersector.h"
#include "Wm3Ray3.h"
#include "Wm3Sphere3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrRay3Sphere3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Sphere3 (const Ray3<Real>& rkRay, const Sphere3<Real>& rkSphere);

    // object access
    const Ray3<Real>& GetRay () const;
    const Sphere3<Real>& GetSphere () const;

    // test-intersection query
    virtual bool Test ();

    // find-intersection query
    virtual bool Find ();
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetRayT (int i) const;

private:
    // the objects to intersect
    const Ray3<Real>& m_rkRay;
    const Sphere3<Real>& m_rkSphere;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
    Real m_afRayT[2];
};

typedef IntrRay3Sphere3<float> IntrRay3Sphere3f;
typedef IntrRay3Sphere3<double> IntrRay3Sphere3d;

}

#endif
