// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE2CIRCLE2_H
#define WM3INTRLINE2CIRCLE2_H

#include "Wm3Intersector.h"
#include "Wm3Line2.h"
#include "Wm3Circle2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine2Circle2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrLine2Circle2 (const Line2<Real>& rkLine,
        const Circle2<Real>& rkCircle);

    // object access
    const Line2<Real>& GetLine () const;
    const Circle2<Real>& GetCircle () const;

    // static intersection query
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Line2<Real>& m_rkLine;
    const Circle2<Real>& m_rkCircle;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];

// internal use (shared by IntrRay2Circle2, IntrSegment2Circle2,
//   IntrLine2Arc2, IntrRay2Arc2, IntrSegment2Arc2)
public:
    static bool Find (const Vector2<Real>& rkOrigin,
        const Vector2<Real>& rkDirection, const Vector2<Real>& rkCenter,
        Real fRadius, int& riRootCount, Real afT[2]);
};

typedef IntrLine2Circle2<float> IntrLine2Circle2f;
typedef IntrLine2Circle2<double> IntrLine2Circle2d;

}

#endif
