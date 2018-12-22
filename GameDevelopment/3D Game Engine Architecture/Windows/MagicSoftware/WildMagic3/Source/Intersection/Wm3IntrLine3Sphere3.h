// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3SPHERE3_H
#define WM3INTRLINE3SPHERE3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Sphere3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Sphere3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Sphere3 (const Line3<Real>& rkLine,
        const Sphere3<Real>& rkSphere);

    // object access
    const Line3<Real>& GetLine () const;
    const Sphere3<Real>& GetSphere () const;

    // test-intersection query
    virtual bool Test ();

    // find-intersection query
    virtual bool Find ();
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;
    Real GetLineT (int i) const;

private:
    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Sphere3<Real>& m_rkSphere;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
    Real m_afLineT[2];
};

typedef IntrLine3Sphere3<float> IntrLine3Sphere3f;
typedef IntrLine3Sphere3<double> IntrLine3Sphere3d;

}

#endif
