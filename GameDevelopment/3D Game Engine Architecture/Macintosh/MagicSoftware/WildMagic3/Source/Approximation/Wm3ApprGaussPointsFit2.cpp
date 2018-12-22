// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprGaussPointsFit2.h"
#include "Wm3Eigen.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Box2<Real> GaussPointsFit2 (int iQuantity, const Vector2<Real>* akPoint,
    const bool* abValid)
{
    Box2<Real> kBox(Vector2<Real>::ZERO,Vector2<Real>::UNIT_X,
        Vector2<Real>::UNIT_Y,(Real)1.0,(Real)1.0);

    int i;
    Real fInvQuantity;
    Vector2<Real> kDiff;

    // compute the mean of the points
    if ( !abValid )
    {
        kBox.Center = akPoint[0];
        for (i = 1; i < iQuantity; i++)
        {
            kBox.Center += akPoint[i];
        }
        fInvQuantity = ((Real)1.0)/iQuantity;
    }
    else
    {
        int iValidQuantity = 0;
        for (i = 0; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                kBox.Center += akPoint[i];
                iValidQuantity++;
            }
        }
        if ( iValidQuantity == 0 )
        {
            kBox.Extent[0] = (Real)-1.0;
            kBox.Extent[1] = (Real)-1.0;
            kBox.Extent[2] = (Real)-1.0;
            return kBox;
        }
        fInvQuantity = ((Real)1.0)/iValidQuantity;
    }
    kBox.Center *= fInvQuantity;

    // compute the covariance matrix of the points
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumYY = (Real)0.0;
    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++)
        {
            kDiff = akPoint[i] - kBox.Center;
            fSumXX += kDiff.X()*kDiff.X();
            fSumXY += kDiff.X()*kDiff.Y();
            fSumYY += kDiff.Y()*kDiff.Y();
        }
    }
    else
    {
        for (i = 0; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                kDiff = akPoint[i] - kBox.Center;
                fSumXX += kDiff.X()*kDiff.X();
                fSumXY += kDiff.X()*kDiff.Y();
                fSumYY += kDiff.Y()*kDiff.Y();
            }
        }
    }

    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumYY *= fInvQuantity;

    // setup the eigensolver
    Eigen<Real> kES(2);
    kES(0,0) = fSumXX;
    kES(0,1) = fSumXY;
    kES(1,0) = fSumXY;
    kES(1,1) = fSumYY;
    kES.IncrSortEigenStuff2();

    for (i = 0; i < 2; i++)
    {
        kBox.Extent[i] = kES.GetEigenvalue(i);
        kES.GetEigenvector(i,kBox.Axis[i]);
    }

    return kBox;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
Box2<float> GaussPointsFit2<float> (int, const Vector2<float>*,
    const bool*);

// double
template WM3_ITEM
Box2<double> GaussPointsFit2<double> (int, const Vector2<double>*,
    const bool*);
//----------------------------------------------------------------------------
}
