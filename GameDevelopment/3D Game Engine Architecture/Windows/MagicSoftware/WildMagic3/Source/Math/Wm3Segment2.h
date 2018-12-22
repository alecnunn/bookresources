// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SEGMENT2_H
#define WM3SEGMENT2_H

#include "Wm3Vector2.h"

namespace Wm3
{

template <class Real>
class Segment2
{
public:
    // The segment is represented as P+t*D, where P is the segment origin,
    // D is a unit-length direction vector and |t| <= e.  The value e is
    // referred to as the extent of the segment.  The end points of the
    // segment are P-e*D and P+e*D.  The user must ensure that the direction
    // vector is unit-length.  The representation for a segment is analogous
    // to that for an oriented bounding box.  P is the center, D is the
    // axis direction, and e is the extent.

    // construction
    Segment2 ();  // uninitialized
    Segment2 (const Vector2<Real>& rkOrigin, const Vector2<Real>& rkDirection,
        Real fExtent);

    // end points
    Vector2<Real> GetPosEnd () const;  // P+e*D
    Vector2<Real> GetNegEnd () const;  // P-e*D

    Vector2<Real> Origin, Direction;
    Real Extent;
};

#include "Wm3Segment2.inl"

typedef Segment2<float> Segment2f;
typedef Segment2<double> Segment2d;

}

#endif
