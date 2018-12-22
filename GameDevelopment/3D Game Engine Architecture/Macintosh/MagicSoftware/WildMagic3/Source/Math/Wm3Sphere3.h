// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SPHERE3_H
#define WM3SPHERE3_H

#include "Wm3Vector3.h"

namespace Wm3
{

template <class Real>
class Sphere3
{
public:
    // The sphere is represented as |X-C| = R where C is the center and R is
    // the radius.

    Sphere3 ();  // uninitialized
    Sphere3 (const Vector3<Real>& rkCenter, Real fRadius);
    Sphere3 (const Sphere3& rkSphere);

    // assignment
    Sphere3& operator= (const Sphere3& rkSphere);

    Vector3<Real> Center;
    Real Radius;
};

#include "Wm3Sphere3.inl"

typedef Sphere3<float> Sphere3f;
typedef Sphere3<double> Sphere3d;

}

#endif
