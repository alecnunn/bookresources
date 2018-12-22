// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPRPOLYFIT4_H
#define WM3APPRPOLYFIT4_H

#include "Wm3System.h"

namespace Wm3
{

// The samples are (x[i],y[i],z[i],w[i]) for 0 <= i < S.  Think of w as a
// function of x, y, and z, say w = f(x,y,z).  The function fits the samples
// with a polynomial of degree dx in x, degree dy in y, and degree dz in z,
// say
//   w = sum_{i=0}^{dx} sum_{j=0}^{dy} sum_{k=0}^{dz} c[i][j][k]*x^i*y^j*z^k
// The method is a least-squares fitting algorithm.  The return array stores
// the c[i][j][k] values according to
//   returned[i+(dx+1)*(j+(dy+1)*k)] = c[i][j][k]
// for a total of (dx+1)*(dy+1)*(dz+1) coefficients.  The caller is
// responsible for deleting the input arrays if they were dynamically
// allocated.  The caller is also responsible for deleting the returned array.
template <class Real> WM3_ITEM
Real* PolyFit4 (int iSamples, const Real* afX, const Real* afY,
    const Real* afZ, const Real* afW, int iXDegree, int iYDegree,
    int iZDegree);

}

#endif
