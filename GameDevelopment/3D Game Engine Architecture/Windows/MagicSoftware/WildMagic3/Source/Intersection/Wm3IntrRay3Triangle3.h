// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRRAY3TRIANGLE3_H
#define WM3INTRRAY3TRIANGLE3_H

#include "Wm3Intersector.h"
#include "Wm3Ray3.h"
#include "Wm3Triangle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrRay3Triangle3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Triangle3 (const Ray3<Real>& rkRay,
        const Triangle3<Real>& rkTriangle);

    // object access
    const Ray3<Real>& GetRay () const;
    const Triangle3<Real>& GetTriangle () const;

    // test-intersection query
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    //   P = origin + t*direction = b0*V0 + b1*V1 + b2*V2
    virtual bool Find ();
    Real GetRayT () const;
    Real GetTriB0 () const;
    Real GetTriB1 () const;
    Real GetTriB2 () const;

private:
    // the objects to intersect
    const Ray3<Real>& m_rkRay;
    const Triangle3<Real>& m_rkTriangle;

    // information about the intersection set
    Real m_fRayT, m_fTriB0, m_fTriB1, m_fTriB2;
};

typedef IntrRay3Triangle3<float> IntrRay3Triangle3f;
typedef IntrRay3Triangle3<double> IntrRay3Triangle3d;

}

#endif
