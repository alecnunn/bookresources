// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprGaussPointsFit3.h"
#include "Wm3Eigen.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Box3<Real> GaussPointsFit3 (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid)
{
    Box3<Real> kBox(Vector3<Real>::ZERO,Vector3<Real>::UNIT_X,
        Vector3<Real>::UNIT_Y,Vector3<Real>::UNIT_Z,(Real)1.0,(Real)1.0,
        (Real)1.0);

    int i;
    Real fInvQuantity;
    Vector3<Real> kDiff;

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
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumXZ = (Real)0.0;
    Real fSumYY = (Real)0.0, fSumYZ = (Real)0.0, fSumZZ = (Real)0.0;
    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++)
        {
            kDiff = akPoint[i] - kBox.Center;
            fSumXX += kDiff.X()*kDiff.X();
            fSumXY += kDiff.X()*kDiff.Y();
            fSumXZ += kDiff.X()*kDiff.Z();
            fSumYY += kDiff.Y()*kDiff.Y();
            fSumYZ += kDiff.Y()*kDiff.Z();
            fSumZZ += kDiff.Z()*kDiff.Z();
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
                fSumXZ += kDiff.X()*kDiff.Z();
                fSumYY += kDiff.Y()*kDiff.Y();
                fSumYZ += kDiff.Y()*kDiff.Z();
                fSumZZ += kDiff.Z()*kDiff.Z();
            }
        }
    }

    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumXZ *= fInvQuantity;
    fSumYY *= fInvQuantity;
    fSumYZ *= fInvQuantity;
    fSumZZ *= fInvQuantity;

    // setup the eigensolver
    Eigen<Real> kES(3);
    kES(0,0) = fSumXX;
    kES(0,1) = fSumXY;
    kES(0,2) = fSumXZ;
    kES(1,0) = fSumXY;
    kES(1,1) = fSumYY;
    kES(1,2) = fSumYZ;
    kES(2,0) = fSumXZ;
    kES(2,1) = fSumYZ;
    kES(2,2) = fSumZZ;
    kES.IncrSortEigenStuff3();

    for (i = 0; i < 3; i++)
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
Box3<float> GaussPointsFit3<float> (int, const Vector3<float>*,
    const bool*);

// double
template WM3_ITEM
Box3<double> GaussPointsFit3<double> (int, const Vector3<double>*,
    const bool*);
//----------------------------------------------------------------------------
}
