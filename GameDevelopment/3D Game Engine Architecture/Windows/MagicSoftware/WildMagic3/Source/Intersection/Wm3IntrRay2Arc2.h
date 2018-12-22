// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRRAY2ARC2_H
#define WM3INTRRAY2ARC2_H

#include "Wm3Intersector.h"
#include "Wm3Ray2.h"
#include "Wm3Arc2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrRay2Arc2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrRay2Arc2 (const Ray2<Real>& rkRay, const Arc2<Real>& rkArc);

    // object access
    const Ray2<Real>& GetRay () const;
    const Arc2<Real>& GetArc () const;

    // static intersection query
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Ray2<Real>& m_rkRay;
    const Arc2<Real>& m_rkArc;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[2];
};

typedef IntrRay2Arc2<float> IntrRay2Arc2f;
typedef IntrRay2Arc2<double> IntrRay2Arc2d;

}

#endif
