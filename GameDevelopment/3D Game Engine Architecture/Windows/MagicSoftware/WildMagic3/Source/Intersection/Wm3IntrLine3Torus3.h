// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3TORUS3_H
#define WM3INTRLINE3TORUS3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Torus3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Torus3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Torus3 (const Line3<Real>& rkLine, const Torus3<Real>& rkTorus);

    // object access
    const Line3<Real>& GetLine () const;
    const Torus3<Real>& GetTorus () const;

    // static intersection query
    virtual bool Find ();

    // the intersection set (quantity is at most 4)
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Torus3<Real>& m_rkTorus;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[4];
};

typedef IntrLine3Torus3<float> IntrLine3Torus3f;
typedef IntrLine3Torus3<double> IntrLine3Torus3d;

}

#endif
