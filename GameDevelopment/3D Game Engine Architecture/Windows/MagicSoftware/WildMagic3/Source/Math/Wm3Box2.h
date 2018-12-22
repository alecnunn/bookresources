// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BOX2_H
#define WM3BOX2_H

#include "Wm3Vector2.h"

namespace Wm3
{

template <class Real>
class Box2
{
public:
    // A box has center C, axis directions U[0] and U[1] (both unit-length
    // vectors), and extents e[0] and e[1] (both nonnegative numbers).  A
    // point X = C+y[0]*U[0]+y[1]*U[1] is inside or on the box whenever
    // |y[i]| <= e[i] for all i.

    // construction
    Box2 ();  // uninitialized
    Box2 (const Vector2<Real>& rkCenter, const Vector2<Real>* akAxis,
        const Real* afExtent);
    Box2 (const Vector2<Real>& rkCenter, const Vector2<Real>& rkAxis0,
        const Vector2<Real>& rkAxis1, Real fExtent0, Real fExtent1);

    void ComputeVertices (Vector2<Real> akVertex[4]) const;

    Vector2<Real> Center;
    Vector2<Real> Axis[2];  // must be an orthonormal set of vectors
    Real Extent[2];         // must be nonnegative
};

#include "Wm3Box2.inl"

typedef Box2<float> Box2f;
typedef Box2<double> Box2d;

}

#endif
