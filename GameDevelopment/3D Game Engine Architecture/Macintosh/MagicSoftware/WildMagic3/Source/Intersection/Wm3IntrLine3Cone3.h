// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3CONE3_H
#define WM3INTRLINE3CONE3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Cone3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Cone3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Cone3 (const Line3<Real>& rkLine, const Cone3<Real>& rkCone);

    // object access
    const Line3<Real>& GetLine () const;
    const Cone3<Real>& GetCone () const;

    // static intersection query
    virtual bool Find ();

    // The intersection set.  The possible intersection types are
    //   IT_EMTPY:   no intersection
    //   IT_POINT:   point[0] is the intersection point
    //   IT_SEGMENT: <point[0],point[1]> is the intersection segment
    //   IT_RAY:     point[0]+t*point[1] is the intersection ray
    int GetQuantity () const;
    const Vector3<Real>& GetPoint (int i) const;

private:
    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Cone3<Real>& m_rkCone;

    // information about the intersection set
    int m_iQuantity;
    Vector3<Real> m_akPoint[2];
};

typedef IntrLine3Cone3<float> IntrLine3Cone3f;
typedef IntrLine3Cone3<double> IntrLine3Cone3d;

}

#endif
