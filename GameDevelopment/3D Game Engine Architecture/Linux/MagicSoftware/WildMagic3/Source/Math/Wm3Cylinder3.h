// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CYLINDER3_H
#define WM3CYLINDER3_H

#include "Wm3Segment3.h"

namespace Wm3
{

template <class Real>
class Cylinder3
{
public:
    // Cylinder line segment has end points C-(H/2)*D and C+(H/2)*D where
    // D is a unit-length vector.  H is infinity for infinite cylinder.

    // construction
    Cylinder3 ();  // uninitialized
    Cylinder3 (const Segment3<Real>& rkSegment, Real fHeight, Real fRadius);

    Segment3<Real> Segment;
    Real Height, Radius;
};

#include "Wm3Cylinder3.inl"

typedef Cylinder3<float> Cylinder3f;
typedef Cylinder3<double> Cylinder3d;

}

#endif
