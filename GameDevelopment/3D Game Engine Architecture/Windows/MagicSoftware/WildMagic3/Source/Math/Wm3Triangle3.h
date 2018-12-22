// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TRIANGLE3_H
#define WM3TRIANGLE3_H

#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class Triangle3
{
public:
    // The triangle is represented as an array of three vertices, V0, V1,
    // and V2.

    // construction
    Triangle3 ();  // uninitialized
    Triangle3 (const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
        const Vector3<Real>& rkV2);
    Triangle3 (const Vector3<Real> akV[3]);

    // distance from the point Q to the triangle
    Real DistanceTo (const Vector3<Real>& rkQ) const;

    Vector3<Real> V[3];
};

#include "Wm3Triangle3.inl"

typedef Triangle3<float> Triangle3f;
typedef Triangle3<double> Triangle3d;

}

#endif
