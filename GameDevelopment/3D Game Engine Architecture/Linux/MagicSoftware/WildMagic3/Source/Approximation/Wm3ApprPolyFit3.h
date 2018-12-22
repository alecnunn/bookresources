// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPRPOLYFIT3_H
#define WM3APPRPOLYFIT3_H

#include "Wm3System.h"

namespace Wm3
{

// The samples are (x[i],y[i],w[i]) for 0 <= i < S.  Think of w as a function
// of x and y, say w = f(x,y).  The function fits the samples with a
// polynomial of degree dx in x and degree dy in y, say
//   w = sum_{i=0}^{dx} sum_{j=0}^{dy} c[i][j]*x^i*y^j
// The method is a least-squares fitting algorithm.  The returned array
// stores the c[i][j] values according to returned[i+(dx+1)*j] = c[i][j]
// for a total of (dx+1)*(dy+1) coefficients.  The caller is responsible for
// deleting the input arrays if they were dynamically allocated.  The caller
// is also responsible for deleting the returned array.
template <class Real> WM3_ITEM
Real* PolyFit3 (int iSamples, const Real* afX, const Real* afY,
    const Real* afW, int iXDegree, int iYDegree);

}

#endif
