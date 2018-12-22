// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprSphereFit3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
bool SphereFit3 (int iQuantity, const Vector3<Real>* akPoint,
    int iMaxIterations, Vector3<Real>& rkCenter, Real& rfRadius)
{
    // compute the average of the data points
    Vector3<Real> kAverage = akPoint[0];
    int i0;
    for (i0 = 1; i0 < iQuantity; i0++)
        kAverage += akPoint[i0];
    Real fInvQuantity = ((Real)1.0)/(Real)iQuantity;
    kAverage *= fInvQuantity;

    // initial guess
    rkCenter = kAverage;

    int i1;
    for (i1 = 0; i1 < iMaxIterations; i1++)
    {
        // update the iterates
        Vector3<Real> kCurrent = rkCenter;

        // compute average L, dL/da, dL/db, dL/dc
        Real fLAverage = (Real)0.0;
        Vector3<Real> kDerLAverage = Vector3<Real>::ZERO;
        for (i0 = 0; i0 < iQuantity; i0++)
        {
            Vector3<Real> kDiff = akPoint[i0] - rkCenter;
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

        rkCenter = kAverage + fLAverage*kDerLAverage;
        rfRadius = fLAverage;

        Vector3<Real> kDiff = rkCenter - kCurrent;
        if ( Math<Real>::FAbs(kDiff.X()) <= Math<Real>::ZERO_TOLERANCE
        &&   Math<Real>::FAbs(kDiff.Y()) <= Math<Real>::ZERO_TOLERANCE
        &&   Math<Real>::FAbs(kDiff.Z()) <= Math<Real>::ZERO_TOLERANCE )
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
bool SphereFit3<float> (int, const Vector3<float>*, int, Vector3<float>&,
    float&);

// double
template WM3_ITEM
bool SphereFit3<double> (int, const Vector3<double>*, int, Vector3<double>&,
    double&);
//----------------------------------------------------------------------------
}
