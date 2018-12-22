// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CIRCLE3_H
#define WM3CIRCLE3_H

#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class Circle3
{
public:
    // Plane containing circle is Dot(N,X-C) = 0 where X is any point in the
    // plane.  Vectors U, V, and N form an orthonormal right-handed set
    // (matrix [U V N] is orthonormal and has determinant 1).  Circle within
    // the plane is parameterized by X = C + R*(cos(A)*U + sin(A)*V) where
    // A is an angle in [0,2*pi).

    // construction
    Circle3 ();  // uninitialized
    Circle3 (const Vector3<Real>& rkCenter, const Vector3<Real>& rkU,
        const Vector3<Real>& rkV, const Vector3<Real>& rkN, Real fRadius);

    Vector3<Real> Center, U, V, N;
    Real Radius;
};

#include "Wm3Circle3.inl"

typedef Circle3<float> Circle3f;
typedef Circle3<double> Circle3d;

}

#endif
