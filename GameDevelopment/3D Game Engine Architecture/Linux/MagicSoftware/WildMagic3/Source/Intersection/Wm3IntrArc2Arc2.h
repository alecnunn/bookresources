// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRARC2ARC2_H
#define WM3INTRARC2ARC2_H

#include "Wm3Intersector.h"
#include "Wm3Arc2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrArc2Arc2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrArc2Arc2 (const Arc2<Real>& rkArc0, const Arc2<Real>& rkArc1);

    // object access
    const Arc2<Real>& GetArc0 () const;
    const Arc2<Real>& GetArc1 () const;

    // static find-intersection query
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q is
    // 0, 1, or 2.  When Q > 0, the interpretation depends on the intersection
    // type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The arcs overlap by more than a point.  The arc of
    //              intersection is accessed by GetIntersectionArc().  Q is
    //              invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Arc2<Real>& GetIntersectionArc () const;

private:
    // the objects to intersect
    const Arc2<Real>& m_rkArc0;
    const Arc2<Real>& m_rkArc1;

    // set of intersection
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
    Arc2<Real> m_kIntersectionArc;
};

typedef IntrArc2Arc2<float> IntrArc2Arc2f;
typedef IntrArc2Arc2<double> IntrArc2Arc2d;

}

#endif
