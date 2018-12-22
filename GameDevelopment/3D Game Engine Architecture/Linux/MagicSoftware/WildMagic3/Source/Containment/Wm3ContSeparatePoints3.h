// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONTSEPARATEPOINTS3_H
#define WM3CONTSEPARATEPOINTS3_H

// Separate two point sets, if possible, by computing a plane for which the
// point sets lie on opposite sides.  The algorithm computes the convex hull
// of the point sets, then uses the method of separating axes to determine if
// the two convex polyhedra are disjoint.  The convex hull calculation is
// O(n*log(n)).  There is a randomized linear approach that takes O(n), but
// I have not yet implemented it.
//
// The return value of the function is 'true' if and only if there is a
// separation.  If 'true', the returned plane is a separating plane.

#include "Wm3Plane3.h"

namespace Wm3
{

// Assumes that both sets have at least 4 noncoplanar points.
template <class Real>
class WM3_ITEM SeparatePoints3
{
public:
    SeparatePoints3 (int iQuantity0, const Vector3<Real>* akVertex0,
        int iQuantity1, const Vector3<Real>* akVertex1,
        Plane3<Real>& rkSeprPlane);

    // Return the result of the constructor call.  If 'true', the output
    // plane rkSeprPlane is valid.
    operator bool ();

private:
    static int OnSameSide (const Plane3<Real>& rkPlane, int iTriangleQuantity,
        const int* aiIndex, const Vector3<Real>* akPoint);

    static int WhichSide (const Plane3<Real>& rkPlane, int iTriangleQuantity,
        const int* aiIndex, const Vector3<Real>* akPoint);

    bool m_bSeparated;
};

typedef SeparatePoints3<float> SeparatePoints3f;
typedef SeparatePoints3<double> SeparatePoints3d;

}

#endif
