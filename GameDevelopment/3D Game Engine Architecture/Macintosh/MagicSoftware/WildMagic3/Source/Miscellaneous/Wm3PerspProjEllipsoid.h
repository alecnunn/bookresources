// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PERSPPROJELLIPSOID_H
#define WM3PERSPPROJELLIPSOID_H

#include "Wm3Ellipse2.h"
#include "Wm3Ellipsoid3.h"
#include "Wm3Plane3.h"

namespace Wm3
{

// Input
//   ellipsoid:   specified by center, axis directions, and extents
//   eye point:   E
//   view plane:  Dot(N,X) = D, planar vectors U and V so that {U,V,N} is an
//                orthonormal set of vectors
// The ellipsoid should be between the eye point and the view plane in the
// sense that all rays from the eye point that intersect the ellipsoid must
// also intersect the view plane.
//
// Output
//   plane origin:       P = E + (D - Dot(N,E))*N
//   projected ellipse:  The projected ellipse coordinates Y = (y0,y1) are the
//                       view plane coordinates of the actual 3D ellipse
//                       points X = P + y0*U + y1*V.

template <class Real> WM3_ITEM
void PerspectiveProjectEllipsoid (const Ellipsoid3<Real>& rkEllipsoid,
    const Vector3<Real>& rkEye, const Plane3<Real>& rkPlane,
    const Vector3<Real>& rkU, const Vector3<Real>& rkV, Vector3<Real>& rkP,
    Ellipse2<Real>& rkEllipse);

}

#endif
