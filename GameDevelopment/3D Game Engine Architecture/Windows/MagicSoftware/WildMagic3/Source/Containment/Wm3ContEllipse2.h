// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTELLIPSE2_H
#define WM3CONTELLIPSE2_H

#include "Wm3Ellipse2.h"
#include "Wm3Line2.h"

namespace Wm3
{

// The input points are fit with a Gaussian distribution.  The center C of the
// ellipsoid is chosen to be the mean of the distribution.  The axes of the
// ellipsoid are chosen to be the eigenvectors of the covariance matrix M.
// The shape of the ellipsoid is determined by the absolute values of the
// eigenvalues.
//
// WARNING.  The construction is ill-conditioned if the points are (nearly)
// collinear.  In this case M has a (nearly) zero eigenvalue, so inverting M
// is problematic.
template <class Real> WM3_ITEM
Ellipse2<Real> ContEllipse (int iQuantity, const Vector2<Real>* akPoint);

// Project an ellipse onto a line.  The projection interval is [min,max]
// and corresponds to the line segment P+t*D, where min <= t <= max.
template <class Real> WM3_ITEM
void ProjectEllipse (const Ellipse2<Real>& rkEllipse,
    const Line2<Real>& rkLine, Real& rfMin, Real& rfMax);

// Construct a bounding ellipsoid for the two input ellipsoids.
template <class Real> WM3_ITEM
const Ellipse2<Real> MergeEllipses (const Ellipse2<Real>& rkEllipse0,
    const Ellipse2<Real>& rkEllipse1);

}

#endif
