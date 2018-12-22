// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRCAPSULE3CAPSULE3_H
#define WM3INTRCAPSULE3CAPSULE3_H

#include "Wm3Intersector.h"
#include "Wm3Capsule3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrCapsule3Capsule3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrCapsule3Capsule3 (const Capsule3<Real>& rkCapsule0,
        const Capsule3<Real>& rkCapsule1);

    // object access
    const Capsule3<Real>& GetCapsule0 () const;
    const Capsule3<Real>& GetCapsule1 () const;

    // static test-intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Capsule3<Real>& m_rkCapsule0;
    const Capsule3<Real>& m_rkCapsule1;
};

typedef IntrCapsule3Capsule3<float> IntrCapsule3Capsule3f;
typedef IntrCapsule3Capsule3<double> IntrCapsule3Capsule3d;

}

#endif
