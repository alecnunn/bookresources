// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SINGLECURVE2_H
#define WM3SINGLECURVE2_H

#include "Wm3Curve2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM SingleCurve2 : public Curve2<Real>
{
public:
    // abstract base class
    SingleCurve2 (Real fTMin, Real fTMax);

    // length-from-time and time-from-length
    virtual Real GetLength (Real fT0, Real fT1) const;
    virtual Real GetTime (Real fLength, int iIterations = 32,
        Real fTolerance = (Real)1e-06) const;

protected:
    static Real GetSpeedWithData (Real fTime, void* pvData);
};

typedef SingleCurve2<float> SingleCurve2f;
typedef SingleCurve2<double> SingleCurve2d;

}

#endif
