// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSEGMENT2CIRCLE2_H
#define WM3INTRSEGMENT2CIRCLE2_H

#include "Wm3Intersector.h"
#include "Wm3Segment2.h"
#include "Wm3Circle2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSegment2Circle2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrSegment2Circle2 (const Segment2<Real>& rkSegment,
        const Circle2<Real>& rkCircle);

    // object access
    const Segment2<Real>& GetSegment () const;
    const Circle2<Real>& GetCircle () const;

    // static intersection query
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Segment2<Real>& m_rkSegment;
    const Circle2<Real>& m_rkCircle;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrSegment2Circle2<float> IntrSegment2Circle2f;
typedef IntrSegment2Circle2<double> IntrSegment2Circle2d;

}

#endif
