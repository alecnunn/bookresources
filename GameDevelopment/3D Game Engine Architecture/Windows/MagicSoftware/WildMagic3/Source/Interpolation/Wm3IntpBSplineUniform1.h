// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTPBSPLINEUNIFORM1_H
#define WM3INTPBSPLINEUNIFORM1_H

#include "Wm3IntpBSplineUniform.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntpBSplineUniform1 : public IntpBSplineUniform<Real>
{
public:
    // Construction.  IntpBSplineUniform1 accepts responsibility for
    // deleting the input array afData.
    IntpBSplineUniform1 (int iDegree, int iDim, Real* afData);

    // spline evaluation for function interpolation (no derivatives)
    Real operator() (Real fX);
    virtual Real operator() (Real* afX);

    // spline evaluation, derivative count given in iDx and aiDx[]
    Real operator() (int iDx, Real fX);
    virtual Real operator() (int* aiDx, Real* afX);

private:
    void EvaluateUnknownData ();
    void ComputeIntermediate ();
};

typedef IntpBSplineUniform1<float> IntpBSplineUniform1f;
typedef IntpBSplineUniform1<double> IntpBSplineUniform1d;

}

#endif
