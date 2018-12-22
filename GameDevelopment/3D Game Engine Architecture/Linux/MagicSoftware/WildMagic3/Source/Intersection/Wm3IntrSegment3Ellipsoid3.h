// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSEGMENT3ELLIPSOID3_H
#define WM3INTRSEGMENT3ELLIPSOID3_H

#include "Wm3Intersector.h"
#include "Wm3Segment3.h"
#include "Wm3Ellipsoid3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSegment3Ellipsoid3 :
    public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Ellipsoid3 (const Segment3<Real>& rkSegment,
        const Ellipsoid3<Real>& rkEllipsoid);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Ellipsoid3<Real>& GetEllipsoid () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // The intersection set.  The ellipsoid is considered a solid, so if the
    // segment is stricly inside the ellipsoid, the intersection type is
    // IT_SEGMENT and the function GetPoint(i) returns the segment end
    // points.  If you want the ellipsoid to be only the surface itself, and
    // you want a "no intersection" result when the segment is strictly
    // inside the ellipsoid, just compare the segment end points to the
    // values from GetPoint(i) whenever IT_SEGMENT is the type.  If they
    // are the same, consider this a "no intersection".
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Segment3<Real>& m_rkSegment;
    const Ellipsoid3<Real>& m_rkEllipsoid;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrSegment3Ellipsoid3<float> IntrSegment3Ellipsoid3f;
typedef IntrSegment3Ellipsoid3<double> IntrSegment3Ellipsoid3d;

}

#endif
