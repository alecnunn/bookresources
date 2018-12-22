// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPRPLANEFIT3_H
#define WM3APPRPLANEFIT3_H

#include "Wm3Plane3.h"

namespace Wm3
{

// Least-squares fit of a plane to (x,y,f(x,y)) data by using distance
// measurements in the z-direction.  The resulting plane is represented by
// z = A*x + B*y + C.  The return value is 'false' if the 3x3 coefficient
// matrix in the linear system that defines A, B, and C is (nearly) singular.
// In this case, A, B, and C are returned as MAX_REAL.  If you want only a
// subset of the input points, provide an array of Boolean values, where
// abValid[i] is set to 'true' if you want akPoint[i] to be included in the
// calculations.  If you provide a Boolean array with all 'false' values, the
// returned values for A, B, and C are MAX_REAL.
template <class Real> WM3_ITEM
bool HeightPlaneFit3 (int iQuantity, const Vector3<Real>* akPoint,
    Real& rfA, Real& rfB, Real& rfC, const bool* abValid = 0);

// Least-squares fit of a plane to (x,y,z) data by using distance measurements
// orthogonal to the proposed plane. If you want only a subset of the input
// points, provide an array of Boolean values, where abValid[i] is set to
// 'true' if you want akPoint[i] to be included in the calculations.  If you
// provide a Boolean array with all 'false' values, the returned plane has
// normal (0,0,0) and constant 0.
template <class Real> WM3_ITEM
Plane3<Real> OrthogonalPlaneFit3 (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid = 0);

}

#endif
