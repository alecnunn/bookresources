// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3CAPSULE3_H
#define WM3INTRLINE3CAPSULE3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Capsule3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Capsule3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Capsule3 (const Line3<Real>& rkLine,
        const Capsule3<Real>& rkCapsule);

    // object access
    const Line3<Real>& GetLine () const;
    const Capsule3<Real>& GetCapsule () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Capsule3<Real>& m_rkCapsule;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];

// internal use (shared by IntrRay3Capsule3 and IntrSegment3Capsule3)
public:
    static int Find (const Vector3<Real>& rkOrigin,
        const Vector3<Real>& rkDir, const Capsule3<Real>& rkCapsule,
        Real afT[2]);
};

typedef IntrLine3Capsule3<float> IntrLine3Capsule3f;
typedef IntrLine3Capsule3<double> IntrLine3Capsule3d;

}

#endif
