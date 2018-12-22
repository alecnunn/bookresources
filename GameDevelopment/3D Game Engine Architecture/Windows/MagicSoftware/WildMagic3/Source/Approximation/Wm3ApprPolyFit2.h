// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3APPRPOLYFIT2_H
#define WM3APPRPOLYFIT2_H

#include "Wm3System.h"

namespace Wm3
{

// The samples are (x[i],w[i]) for 0 <= i < S.  Think of w as a function of
// x, say w = f(x).  The function fits the samples with a polynomial of
// degree d, say w = sum_{i=0}^d c[i]*x^i.  The method is a least-squares
// fitting algorithm.  The returned array stores the coefficients c[i] for
// 0 <= i <= d.  The caller is responsible for deleting the input arrays if
// they were dynamically allocated.  The caller is also responsible for
// deleting the returned array.

template <class Real> WM3_ITEM
Real* PolyFit2 (int iSamples, const Real* afX, const Real* afW, int iXDegree);

}

#endif
