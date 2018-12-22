// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTBOX2_H
#define WM3CONTBOX2_H

#include "Wm3Box2.h"

namespace Wm3
{

// Compute the smallest axis-aligned bounding box of the points.  If you want
// only a subset of the input points, provide an array of Boolean values,
// where abValid[i] is set to 'true' if you want akPoint[i] to be included in
// the calculations.  If you provide a Boolean array with all 'false' values,
// then the returned box has center (0,0), axes (1,0) and (0,1), and extents
// all -1.
template <class Real> WM3_ITEM
Box2<Real> ContAlignedBox (int iQuantity, const Vector2<Real>* akPoint,
    const bool* abValid = 0);

// Compute an oriented bounding box of the points.  The box center is the
// average of the points.  The box axes are the eigenvectors of the covariance
// matrix.  If you want only a subset of the input points, provide an array of
// Boolean values, where abValid[i] is set to 'true' if you want akPoint[i] to
// be included in the calculations.  If you provide a Boolean array with all
// 'false' values, then the returned box has center (0,0), axes (1,0) and
// (0,1, and extents all -1.
template <class Real> WM3_ITEM
Box2<Real> ContOrientedBox (int iQuantity, const Vector2<Real>* akPoint,
    const bool* abValid = 0);

// Compute a minimum area oriented box containing the specified points.  The
// algorithm uses the rotating calipers method.  If the input points represent
// a counterclockwise-ordered polygon, set bIsConvexPolygon to 'true'.
template <class Real> WM3_ITEM
Box2<Real> ContMinBox (int iQuantity, const Vector2<Real>* akPoint,
    bool bIsConvexPolygon);

// Test for containment.  Let X = C + y0*U0 + y1*U1 where C is the box center
// and U0 and U1 are the orthonormal axes of the box.  X is in the box if
// |y_i| <= E_i for all i where E_i are the extents of the box.
template <class Real> WM3_ITEM
bool InBox (const Vector2<Real>& rkPoint, const Box2<Real>& rkBox);

// Construct an oriented box that contains two other oriented boxes.  The
// result is not guaranteed to be the minimum volume box containing the
// input boxes.
template <class Real> WM3_ITEM
Box2<Real> MergeBoxes (const Box2<Real>& rkBox0, const Box2<Real>& rkBox1);

}

#endif
