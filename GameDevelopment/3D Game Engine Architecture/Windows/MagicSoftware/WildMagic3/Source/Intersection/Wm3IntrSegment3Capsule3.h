// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSEGMENT3CAPSULE3_H
#define WM3INTRSEGMENT3CAPSULE3_H

#include "Wm3Intersector.h"
#include "Wm3Segment3.h"
#include "Wm3Capsule3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSegment3Capsule3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSegment3Capsule3 (const Segment3<Real>& rkSegment,
        const Capsule3<Real>& rkCapsule);

    // object access
    const Segment3<Real>& GetSegment () const;
    const Capsule3<Real>& GetCapsule () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetParameter (int i) const;  // segment parameters for points

private:
    // the objects to intersect
    const Segment3<Real>& m_rkSegment;
    const Capsule3<Real>& m_rkCapsule;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
    Real m_afParameter[2];
};

typedef IntrSegment3Capsule3<float> IntrSegment3Capsule3f;
typedef IntrSegment3Capsule3<double> IntrSegment3Capsule3d;

}

#endif
