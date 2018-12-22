// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRBOX2CIRCLE2_H
#define WM3INTRBOX2CIRCLE2_H

#include "Wm3Intersector.h"
#include "Wm3Box2.h"
#include "Wm3Circle2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrBox2Circle2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrBox2Circle2 (const Box2<Real>& rkBox, const Circle2<Real>& rkCircle);

    // object access
    const Box2<Real>& GetBox () const;
    const Circle2<Real>& GetCircle () const;

    // static test-intersection query
    virtual bool Test ();

    // dynamic find-intersection query
    virtual bool Find (Real fTMax, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);

    // intersection set for dynamic find-intersection query
    const Vector2<Real>& GetContactPoint () const;

private:
    // Support for dynamic Find.  Both functions return -1 if the objects are
    // initially intersecting, 0 if no intersection, or +1 if they intersect
    // at some positive time.
    int TestVertexRegion (Real fCx, Real fCy, Real fVx, Real fVy, Real fEx,
        Real fEy, Real& rfIx, Real& rfIy);

    int TestEdgeRegion (Real fCx, Real fCy, Real fVx, Real fVy, Real fEx,
        Real fEy, Real& rfIx, Real& rfIy);

    // the objects to intersect
    const Box2<Real>& m_rkBox;
    const Circle2<Real>& m_rkCircle;

    // point of intersection
    Vector2<Real> m_kContactPoint;
};

typedef IntrBox2Circle2<float> IntrBox2Circle2f;
typedef IntrBox2Circle2<double> IntrBox2Circle2d;

}

#endif
