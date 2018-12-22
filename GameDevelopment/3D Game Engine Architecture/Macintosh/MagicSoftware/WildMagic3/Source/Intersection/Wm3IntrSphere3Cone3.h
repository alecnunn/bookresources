// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSPHERE3CONE3_H
#define WM3INTRSPHERE3CONE3_H

#include "Wm3Intersector.h"
#include "Wm3Sphere3.h"
#include "Wm3Cone3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSphere3Cone3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSphere3Cone3 (const Sphere3<Real>& rkSphere,
        const Cone3<Real>& rkCone);

    // object access
    const Sphere3<Real>& GetSphere () const;
    const Cone3<Real>& GetCone () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // In the static find-intersection query, if an intersection occurs
    // between the sphere and cone, it is potentially an infinite set.  The
    // intersection point closest to the cone vertex is returned by this
    // function.
    const Vector3<Real>& GetPoint () const;

private:
    // the objects to intersect
    const Sphere3<Real>& m_rkSphere;
    const Cone3<Real>& m_rkCone;

    // closest intersection point to cone vertex
    Vector3<Real> m_kPoint;
};

typedef IntrSphere3Cone3<float> IntrSphere3Cone3f;
typedef IntrSphere3Cone3<double> IntrSphere3Cone3d;

}

#endif
