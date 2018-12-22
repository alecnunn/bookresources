// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3BOX3_H
#define WM3INTRLINE3BOX3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Box3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Box3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Box3 (const Line3<Real>& rkLine, const Box3<Real>& rkBox);

    // object access
    const Line3<Real>& GetLine () const;
    const Box3<Real>& GetBox () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // the intersection set
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    static bool Clip (Real fDenom, Real fNumer, Real& rfT0, Real& rfT1);

    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Box3<Real>& m_rkBox;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];

// internal use (shared by IntrRay3Box3 and IntrSegment3Box3)
public:
    static bool DoClipping (Real fT0, Real fT1, const Vector3<Real>& rkOrigin,
        const Vector3<Real>& rkDirection, const Box3<Real>& rkBox,
        int& riQuantity, Vector3<Real> akPoint[2], int& riIntrType);
};

typedef IntrLine3Box3<float> IntrLine3Box3f;
typedef IntrLine3Box3<double> IntrLine3Box3d;

}

#endif
