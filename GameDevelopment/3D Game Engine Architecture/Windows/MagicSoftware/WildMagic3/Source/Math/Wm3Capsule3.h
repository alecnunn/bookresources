// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CAPSULE3_H
#define WM3CAPSULE3_H

#include "Wm3Segment3.h"

namespace Wm3
{

template <class Real>
class Capsule3
{
public:
    // construction
    Capsule3 ();  // uninitialized
    Capsule3 (const Segment3<Real>& rkSegment, Real fRadius);

    Segment3<Real> Segment;
    Real Radius;
};

#include "Wm3Capsule3.inl"

typedef Capsule3<float> Capsule3f;
typedef Capsule3<double> Capsule3d;

}

#endif
