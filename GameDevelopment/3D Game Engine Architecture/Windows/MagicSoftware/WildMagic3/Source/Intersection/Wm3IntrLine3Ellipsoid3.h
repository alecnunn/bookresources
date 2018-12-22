// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3ELLIPSOID3_H
#define WM3INTRLINE3ELLIPSOID3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Ellipsoid3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Ellipsoid3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Ellipsoid3 (const Line3<Real>& rkLine,
        const Ellipsoid3<Real>& rkEllipsoid);

    // object access
    const Line3<Real>& GetLine () const;
    const Ellipsoid3<Real>& GetEllipsoid () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Ellipsoid3<Real>& m_rkEllipsoid;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrLine3Ellipsoid3<float> IntrLine3Ellipsoid3f;
typedef IntrLine3Ellipsoid3<double> IntrLine3Ellipsoid3d;

}

#endif
