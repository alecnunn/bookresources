// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRTRIANGLE3CONE3_H
#define WM3INTRTRIANGLE3CONE3_H

#include "Wm3Intersector.h"
#include "Wm3Triangle3.h"
#include "Wm3Cone3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrTriangle3Cone3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrTriangle3Cone3 (const Triangle3<Real>& rkTriangle,
        const Cone3<Real>& rkCone);

    // object access
    const Triangle3<Real>& GetTriangle () const;
    const Cone3<Real>& GetCone () const;

    // static query
    virtual bool Test ();

private:
    // the objects to intersect
    const Triangle3<Real>& m_rkTriangle;
    const Cone3<Real>& m_rkCone;
};

typedef IntrTriangle3Cone3<float> IntrTriangle3Cone3f;
typedef IntrTriangle3Cone3<double> IntrTriangle3Cone3d;

}

#endif
