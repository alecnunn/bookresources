// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTRAY3SEGMENT3_H
#define WM3DISTRAY3SEGMENT3_H

#include "Wm3Distance.h"
#include "Wm3Ray3.h"
#include "Wm3Segment3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistRay3Segment3 : public Distance<Real,Vector3<Real> >
{
public:
    DistRay3Segment3 (const Ray3<Real>& rkRay,
        const Segment3<Real>& rkSegment);

    // object access
    const Ray3<Real>& GetRay () const;
    const Segment3<Real>& GetSegment () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Ray3<Real>& m_rkRay;
    const Segment3<Real>& m_rkSegment;
};

typedef DistRay3Segment3<float> DistRay3Segment3f;
typedef DistRay3Segment3<double> DistRay3Segment3d;

}

#endif
