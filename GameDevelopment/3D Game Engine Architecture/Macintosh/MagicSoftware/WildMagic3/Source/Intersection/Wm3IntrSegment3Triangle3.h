// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSEGMENT3TRIANGLE3_H
#define WM3INTRSEGMENT3TRIANGLE3_H

#include "Wm3Intersector.h"
#include "Wm3Segment3.h"
#include "Wm3Triangle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSegment3Triangle3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Triangle3 (const Segment3<Real>& rkSegment,
        const Triangle3<Real>& rkTriangle);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Triangle3<Real>& GetTriangle () const;

    // test-intersection query
    virtual bool Test ();

    // Find-intersection query.  The point of intersection is
    //   P = origin + t*direction = b0*V0 + b1*V1 + b2*V2
    virtual bool Find ();
    Real GetSegmentT () const;
    Real GetTriB0 () const;
    Real GetTriB1 () const;
    Real GetTriB2 () const;

private:
    // the objects to intersect
    const Segment3<Real>& m_rkSegment;
    const Triangle3<Real>& m_rkTriangle;

    // information about the intersection set
    Real m_fSegmentT, m_fTriB0, m_fTriB1, m_fTriB2;
};

typedef IntrSegment3Triangle3<float> IntrSegment3Triangle3f;
typedef IntrSegment3Triangle3<double> IntrSegment3Triangle3d;

}

#endif
