// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSPHERE3SPHERE3_H
#define WM3INTRSPHERE3SPHERE3_H

#include "Wm3Intersector.h"
#include "Wm3Sphere3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSphere3Sphere3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSphere3Sphere3 (const Sphere3<Real>& rkSphere0,
        const Sphere3<Real>& rkSphere1);

    // object access
    const Sphere3<Real>& GetSphere0 () const;
    const Sphere3<Real>& GetSphere1 () const;

    // static test-intersection query
    virtual bool Test ();

    // static find-intersection query
    virtual bool Find ();

    // intersection set for static find-intersection query
    const Vector3<Real>& GetCenter () const;
    const Vector3<Real>& GetUAxis () const;
    const Vector3<Real>& GetVAxis () const;
    const Vector3<Real>& GetNormal () const;
    Real GetRadius () const;

    // dynamic test-intersection query
    virtual bool Test (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // dynamic find-intersection query
    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // intersection set for dynamic find-intersection query
    const Vector3<Real>& GetContactPoint () const;

private:
    // the objects to intersect
    const Sphere3<Real>& m_rkSphere0;
    const Sphere3<Real>& m_rkSphere1;

    // Circle of intersection for static spheres.  The center is C and lies
    // on a plane spanned by the unit-length, orthogonal vectors U and V.
    // The plane normal is a unit-length vector N.  The radius of the circle
    // in that plane is R.
    Vector3<Real> m_kCenter, m_kUAxis, m_kVAxis, m_kNormal;
    Real m_fRadius;

    // Point of intersection for dynamic spheres.
    Vector3<Real> m_kContactPoint;
};

typedef IntrSphere3Sphere3<float> IntrSphere3Sphere3f;
typedef IntrSphere3Sphere3<double> IntrSphere3Sphere3d;

}

#endif
