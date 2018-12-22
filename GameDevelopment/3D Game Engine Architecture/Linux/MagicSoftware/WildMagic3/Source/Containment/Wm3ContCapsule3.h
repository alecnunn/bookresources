// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTCAPSULE3_H
#define WM3CONTCAPSULE3_H

#include "Wm3Capsule3.h"
#include "Wm3Sphere3.h"

namespace Wm3
{

// Compute axis of capsule segment using least-squares fit.  Radius is
// maximum distance from points to axis.  Hemispherical caps are chosen
// as close together as possible.  If you want only a subset of the input
// points, provide an array of Boolean values, where abValid[i] is set to
// 'true' if you want akPoint[i] to be included in the calculations.  If you
// provide a Boolean array with all 'false' values, the returned capsule has
// segment with origin (0,0,0) and direction (0,0,0), and radius of zero.
template <class Real> WM3_ITEM
Capsule3<Real> ContCapsule (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid = 0);

// Test for containment of a point by a capsule.
template <class Real> WM3_ITEM
bool InCapsule (const Vector3<Real>& rkPoint,
    const Capsule3<Real>& rkCapsule);

// Test for containment of a sphere by a capsule.
template <class Real> WM3_ITEM
bool InCapsule (const Sphere3<Real>& rkSphere,
    const Capsule3<Real>& rkCapsule);

// Test for containment of a capsule by a capsule.
template <class Real> WM3_ITEM
bool InCapsule (const Capsule3<Real>& rkTestCapsule,
    const Capsule3<Real>& rkCapsule);

// Compute a capsule that contains the input capsules.  The returned capsule
// is not necessarily the one of smallest volume that contains the inputs.
template <class Real> WM3_ITEM
Capsule3<Real> MergeCapsules (const Capsule3<Real>& rkCapsule0,
    const Capsule3<Real>& rkCapsule1);

}

#endif
