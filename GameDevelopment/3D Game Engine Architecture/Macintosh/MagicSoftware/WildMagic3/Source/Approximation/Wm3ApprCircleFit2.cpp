// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprCircleFit2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
bool CircleFit2 (int iQuantity, const Vector2<Real>* akPoint,
    int iMaxIterations, Circle2<Real>& rkCircle)
{
    // compute the average of the data points
    Vector2<Real> kAverage = akPoint[0];
    int i0;
    for (i0 = 1; i0 < iQuantity; i0++)
        kAverage += akPoint[i0];
    Real fInvQuantity = ((Real)1.0)/(Real)iQuantity;
    kAverage *= fInvQuantity;

    // initial guess
    rkCircle.Center = kAverage;

    int i1;
    for (i1 = 0; i1 < iMaxIterations; i1++)
    {
        // update the iterates
        Vector2<Real> kCurrent = rkCircle.Center;

        // compute average L, dL/da, dL/db
        Real fLAverage = (Real)0.0;
        Vector2<Real> kDerLAverage = Vector2<Real>::ZERO;
        for (i0 = 0; i0 < iQuantity; i0++)
        {
            Vector2<Real> kDiff = akPoint[i0] - rkCircle.Center;
            Real fLength = kDiff.Length();
            if ( fLength > Math<Real>::ZERO_TOLERANCE )
            {
                fLAverage += fLength;
                Real fInvLength = ((Real)1.0)/fLength;
                kDerLAverage -= fInvLength*kDiff;
            }
        }
        fLAverage *= fInvQuantity;
        kDerLAverage *= fInvQuantity;

        rkCircle.Center = kAverage + fLAverage*kDerLAverage;
        rkCircle.Radius = fLAverage;

        Vector2<Real> kDiff = rkCircle.Center - kCurrent;
        if ( Math<Real>::FAbs(kDiff.X()) <= Math<Real>::ZERO_TOLERANCE
        &&   Math<Real>::FAbs(kDiff.Y()) <= Math<Real>::ZERO_TOLERANCE )
        {
            break;
        }
    }

    return i1 < iMaxIterations;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
bool CircleFit2<float> (int, const Vector2<float>*, int, Circle2<float>&);

// double
template WM3_ITEM
bool CircleFit2<double> (int, const Vector2<double>*, int, Circle2<double>&);
//----------------------------------------------------------------------------
}
