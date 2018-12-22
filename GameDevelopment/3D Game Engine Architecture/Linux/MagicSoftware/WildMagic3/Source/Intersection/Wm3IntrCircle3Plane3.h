// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRCIRCLE3PLANE3_H
#define WM3INTRCIRCLE3PLANE3_H

#include "Wm3Intersector.h"
#include "Wm3Circle3.h"
#include "Wm3Plane3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrCircle3Plane3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrCircle3Plane3 (const Circle3<Real>& rkCircle,
        const Plane3<Real>& rkPlane);

    // object access
    const Circle3<Real>& GetCircle () const;
    const Plane3<Real>& GetPlane () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // Information about the intersection set.  Only get the specific object
    // of intersection corresponding to the intersection type.  If the type
    // is IT_POINT, use GetPoint(i).  If the type is IT_OTHER, the set is a
    // circle, so use GetIntersectionCircle(), which returns the circle
    // object.
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    const Circle3<Real>& GetIntersectionCircle () const;

protected:
    // the objects to intersect
    const Circle3<Real>& m_rkCircle;
    const Plane3<Real>& m_rkPlane;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrCircle3Plane3<float> IntrCircle3Plane3f;
typedef IntrCircle3Plane3<double> IntrCircle3Plane3d;

}

#endif
