// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3AXISALIGNEDBOX2_H
#define WM3AXISALIGNEDBOX2_H

#include "Wm3System.h"

namespace Wm3
{

template <class Real>
class AxisAlignedBox2
{
public:
    // construction
    AxisAlignedBox2 ();  // uninitialized

    // The caller must ensure that fXMin <= fXMax and fYMin <= fYMax.  The
    // class will not check for validity of the input.
    AxisAlignedBox2 (Real fXMin, Real fXMax, Real fYMin, Real fYMax);

    // Overlap testing is in the strict sense.  If the two boxes are just
    // touching along a common edge, the boxes are reported as overlapping.
    bool HasXOverlap (const AxisAlignedBox2& rkBox) const;
    bool HasYOverlap (const AxisAlignedBox2& rkBox) const;
    bool TestIntersection (const AxisAlignedBox2& rkBox) const;

    // The return value is 'true' if there is overlap.  In this case the
    // intersection is stored in rkIntr.  If the return value is 'false',
    // if there is no overlap.  In this case rkIntr is undefined.
    bool FindIntersection (const AxisAlignedBox2& rkBox,
        AxisAlignedBox2& rkIntr) const;

    Real Min[2], Max[2];
};

#include "Wm3AxisAlignedBox2.inl"

typedef AxisAlignedBox2<float> AxisAlignedBox2f;
typedef AxisAlignedBox2<double> AxisAlignedBox2d;

}

#endif
