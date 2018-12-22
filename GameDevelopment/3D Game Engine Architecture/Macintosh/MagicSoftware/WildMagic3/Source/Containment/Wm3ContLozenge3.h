// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTLOZENGE3_H
#define WM3CONTLOZENGE3_H

#include "Wm3Lozenge3.h"

namespace Wm3
{

// Compute plane of lozenge rectangle using least-squares fit.  Parallel
// planes are chosen close enough together so that all the data points lie
// between them.  The radius is half the distance between the two planes.
// The half-cylinder and quarter-cylinder side pieces are chosen using a
// method similar to that used for fitting by capsules.  If you want only a
// subset of the input points, provide an array of Boolean values, where
// abValid[i] is set to 'true' if you want akPoint[i] to be included in the
// calculations.  If you provide a Boolean array with all 'false' values, the
// returned lozenge has rectangle with center (0,0,0), axes (1,0,0) and
// (0,1,0), and extents all -1, and a radius of zero.
template <class Real> WM3_ITEM
Lozenge3<Real> ContLozenge (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid = 0);

// Test for containment of a point x by a lozenge.
template <class Real> WM3_ITEM
bool InLozenge (const Vector3<Real>& rkPoint,
    const Lozenge3<Real>& rkLozenge);

}

#endif
