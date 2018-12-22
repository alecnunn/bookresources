// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3TETRAHEDRON3_H
#define WM3TETRAHEDRON3_H

#include "Wm3Plane3.h"

namespace Wm3
{

template <class Real>
class Tetrahedron3
{
public:
    // construction
    Tetrahedron3 ();  // uninitialized
    Tetrahedron3 (const Vector3<Real>& rkV0, const Vector3<Real>& rkV1,
        const Vector3<Real>& rkV2, const Vector3<Real>& rkV3);
    Tetrahedron3 (const Vector3<Real> akV[4]);

    // Construct the planes of the faces.  The planes have outer pointing
    // normal vectors, not necessarily unit length.
    void GetPlanes (Plane3<Real> akPlane[4]) const;

    Vector3<Real> V[4];
};

#include "Wm3Tetrahedron3.inl"

typedef Tetrahedron3<float> Tetrahedron3f;
typedef Tetrahedron3<double> Tetrahedron3d;

}

#endif
