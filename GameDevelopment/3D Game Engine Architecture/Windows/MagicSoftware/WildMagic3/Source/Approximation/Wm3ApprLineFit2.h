// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPRLINEFIT2_H
#define WM3APPRLINEFIT2_H

#include "Wm3Line2.h"

namespace Wm3
{

// Least-squares fit of a line to (x,f(x)) data by using distance
// measurements in the y-direction.  The resulting line is represented by
// y = A*x + B.  The return value is 'false' if the 2x2 coefficient matrix
// in the linear system that defines A and B is (nearly) singular.  In this
// case, A and B are returned as MAX_REAL.  If you want only a subset of the
// input points, provide an array of Boolean values, where abValid[i] is set
// to 'true' if you want akPoint[i] to be included in the calculations.  If
// you provide a Boolean array with all 'false' values, the returned values
// for A and B are MAX_REAL.
template <class Real> WM3_ITEM
bool HeightLineFit2 (int iQuantity, const Vector2<Real>* akPoint, Real& rfA,
    Real& rfB, const bool* abValid = 0);

// Least-squares fit of a line to (x,y,z) data by using distance measurements
// orthogonal to the proposed line.  If you want only a subset of the input
// points, provide an array of Boolean values, where abValid[i] is set to
// 'true' if you want akPoint[i] to be included in the calculations.  If you
// provide a Boolean array with all 'false' values, the returned line has
// origin (0,0,0) and direction (0,0,0).
template <class Real> WM3_ITEM
Line2<Real> OrthogonalLineFit2 (int iQuantity, const Vector2<Real>* akPoint,
    const bool* abValid = 0);

}

#endif
