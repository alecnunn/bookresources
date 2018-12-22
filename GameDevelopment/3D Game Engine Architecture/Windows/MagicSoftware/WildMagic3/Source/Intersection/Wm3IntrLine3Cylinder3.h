// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3CYLINDER3_H
#define WM3INTRLINE3CYLINDER3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Cylinder3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Cylinder3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Cylinder3 (const Line3<Real>& rkLine,
        const Cylinder3<Real>& rkCylinder);

    // object access
    const Line3<Real>& GetLine () const;
    const Cylinder3<Real>& GetCylinder () const;

    // static intersection query
    virtual bool Find ();

    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Cylinder3<Real>& m_rkCylinder;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];

// internal use (shared by IntrRay3Cylinder3 and IntrSegment3Cylinder3)
public:
    static int Find (const Vector3<Real>& rkOrigin,
        const Vector3<Real>& rkDir, const Cylinder3<Real>& rkCylinder,
        Real afT[2]);
};

typedef IntrLine3Cylinder3<float> IntrLine3Cylinder3f;
typedef IntrLine3Cylinder3<double> IntrLine3Cylinder3d;

}

#endif
