// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRPLANE3CAPSULE3_H
#define WM3INTRPLANE3CAPSULE3_H

#include "Wm3Intersector.h"
#include "Wm3Plane3.h"
#include "Wm3Capsule3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrPlane3Capsule3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrPlane3Capsule3 (const Plane3<Real>& rkPlane,
        const Capsule3<Real>& rkCapsule);

    // object access
    const Plane3<Real>& GetPlane () const;
    const Capsule3<Real>& GetCapsule () const;

    // static intersection query
    virtual bool Test ();

    // Culling support.  The view frustum is assumed to be on the positive
    // side of the plane.  The capsule is culled if it is on the negative
    // side of the plane.
    bool CapsuleIsCulled () const;

protected:
    // the objects to intersect
    const Plane3<Real>& m_rkPlane;
    const Capsule3<Real>& m_rkCapsule;
};

typedef IntrPlane3Capsule3<float> IntrPlane3Capsule3f;
typedef IntrPlane3Capsule3<double> IntrPlane3Capsule3d;

}

#endif
