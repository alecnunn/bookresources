// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRCIRCLE2CIRCLE2_H
#define WM3INTRCIRCLE2CIRCLE2_H

#include "Wm3Intersector.h"
#include "Wm3Circle2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrCircle2Circle2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrCircle2Circle2 (const Circle2<Real>& rkCircle0,
        const Circle2<Real>& rkCircle1);

    // object access
    const Circle2<Real>& GetCircle0 () const;
    const Circle2<Real>& GetCircle1 () const;

    // static find-intersection query
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q is
    // 0, 1, or 2.  When Q > 0, the interpretation depends on the intersection
    // type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The circles are the same.  One of the circle objects is
    //              returned by GetIntersectionCircle.  Q is invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Circle2<Real>& GetIntersectionCircle () const;

private:
    // the objects to intersect
    const Circle2<Real>& m_rkCircle0;
    const Circle2<Real>& m_rkCircle1;

    // points of intersection
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrCircle2Circle2<float> IntrCircle2Circle2f;
typedef IntrCircle2Circle2<double> IntrCircle2Circle2d;

}

#endif
