// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSEGMENT3CYLINDER3_H
#define WM3INTRSEGMENT3CYLINDER3_H

#include "Wm3Intersector.h"
#include "Wm3Segment3.h"
#include "Wm3Cylinder3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSegment3Cylinder3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Cylinder3 (const Segment3<Real>& rkSegment,
        const Cylinder3<Real>& rkCylinder);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Cylinder3<Real>& GetCylinder () const;

    // static intersection query
    virtual bool Find ();

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Segment3<Real>& m_rkSegment;
    const Cylinder3<Real>& m_rkCylinder;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrSegment3Cylinder3<float> IntrSegment3Cylinder3f;
typedef IntrSegment3Cylinder3<double> IntrSegment3Cylinder3d;

}

#endif
