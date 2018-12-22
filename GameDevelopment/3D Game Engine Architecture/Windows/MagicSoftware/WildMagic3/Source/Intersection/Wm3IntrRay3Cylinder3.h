// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRRAY3CYLINDER3_H
#define WM3INTRRAY3CYLINDER3_H

#include "Wm3Intersector.h"
#include "Wm3Ray3.h"
#include "Wm3Cylinder3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrRay3Cylinder3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Cylinder3 (const Ray3<Real>& rkRay,
        const Cylinder3<Real>& rkCylinder);

    // object access
    const Ray3<Real>& GetRay () const;
    const Cylinder3<Real>& GetCylinder () const;

    // static intersection query
    virtual bool Find ();

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Ray3<Real>& m_rkRay;
    const Cylinder3<Real>& m_rkCylinder;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrRay3Cylinder3<float> IntrRay3Cylinder3f;
typedef IntrRay3Cylinder3<double> IntrRay3Cylinder3d;

}

#endif
