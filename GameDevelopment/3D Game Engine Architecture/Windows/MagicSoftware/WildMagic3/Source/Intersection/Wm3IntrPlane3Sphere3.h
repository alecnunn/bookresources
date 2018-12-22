// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRPLANE3SPHERE3_H
#define WM3INTRPLANE3SPHERE3_H

#include "Wm3Intersector.h"
#include "Wm3Plane3.h"
#include "Wm3Sphere3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrPlane3Sphere3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Sphere3 (const Plane3<Real>& rkPlane,
        const Sphere3<Real>& rkSphere);

    // object access
    const Plane3<Real>& GetPlane () const;
    const Sphere3<Real>& GetSphere () const;

    // static intersection query
    virtual bool Test ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The sphere is culled if it is on the negative
    // side of the plane.
    bool SphereIsCulled () const;

protected:
    // the objects to intersect
    const Plane3<Real>& m_rkPlane;
    const Sphere3<Real>& m_rkSphere;
};

typedef IntrPlane3Sphere3<float> IntrPlane3Sphere3f;
typedef IntrPlane3Sphere3<double> IntrPlane3Sphere3d;

}

#endif
