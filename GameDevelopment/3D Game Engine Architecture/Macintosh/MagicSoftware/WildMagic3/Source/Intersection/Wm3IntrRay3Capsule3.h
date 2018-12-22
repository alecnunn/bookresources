// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRRAY3CAPSULE3_H
#define WM3INTRRAY3CAPSULE3_H

#include "Wm3Intersector.h"
#include "Wm3Ray3.h"
#include "Wm3Capsule3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrRay3Capsule3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Capsule3 (const Ray3<Real>& rkRay,
        const Capsule3<Real>& rkCapsule);

    // object access
    const Ray3<Real>& GetRay () const;
    const Capsule3<Real>& GetCapsule () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Ray3<Real>& m_rkRay;
    const Capsule3<Real>& m_rkCapsule;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrRay3Capsule3<float> IntrRay3Capsule3f;
typedef IntrRay3Capsule3<double> IntrRay3Capsule3d;

}

#endif
