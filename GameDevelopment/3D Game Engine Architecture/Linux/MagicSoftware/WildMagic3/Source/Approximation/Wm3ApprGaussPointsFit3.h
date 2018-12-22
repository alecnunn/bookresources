// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPRGAUSSPOINTSFIT3_H
#define WM3APPRGAUSSPOINTSFIT3_H

#include "Wm3Box3.h"

namespace Wm3
{

// Fit points with a Gaussian distribution.  The center is the mean of the
// points, the axes are the eigenvectors of the covariance matrix, and the
// extents are the eigenvalues of the covariance matrix and are returned in
// increasing order.  The quantites are stored in a Box3<Real> just to have a
// single container.  If you want only a subset of the input points, provide
// an array of Boolean values, where abValid[i] is set to 'true' if you want
// akPoint[i] to be included in the calculations.  If you provide a Boolean
// array with all 'false' values, the returned center is (0,0,0), the axes
// are (1,0,0), (0,1,0), and (0,0,1), and the extents are all -1.
template <class Real> WM3_ITEM
Box3<Real> GaussPointsFit3 (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid = 0);

}

#endif
