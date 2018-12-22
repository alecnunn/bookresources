// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprLineFit3.h"
#include "Wm3Eigen.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Line3<Real> OrthogonalLineFit3 (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid)
{
    Line3<Real> kLine(Vector3<Real>::ZERO,Vector3<Real>::ZERO);
    int i;
    Vector3<Real> kDiff;
    Real fInvQuantity;

    // compute the mean of the points
    if ( !abValid )
    {
        kLine.Origin = akPoint[0];
        for (i = 1; i < iQuantity; i++)
        {
            kLine.Origin += akPoint[i];
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
                kLine.Origin += akPoint[i];
                iValidQuantity++;
            }
        }
        if ( iValidQuantity == 0 )
            return kLine;

        fInvQuantity = ((Real)1.0)/iValidQuantity;
    }
    kLine.Origin *= fInvQuantity;

    // compute the covariance matrix of the points
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumXZ = (Real)0.0;
    Real fSumYY = (Real)0.0, fSumYZ = (Real)0.0, fSumZZ = (Real)0.0;
    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++) 
        {
            kDiff = akPoint[i] - kLine.Origin;
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
                kDiff = akPoint[i] - kLine.Origin;
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

    // set up the eigensolver
    Eigen<Real> kES(3);
    kES(0,0) = fSumYY+fSumZZ;
    kES(0,1) = -fSumXY;
    kES(0,2) = -fSumXZ;
    kES(1,0) = kES(0,1);
    kES(1,1) = fSumXX+fSumZZ;
    kES(1,2) = -fSumYZ;
    kES(2,0) = kES(0,2);
    kES(2,1) = kES(1,2);
    kES(2,2) = fSumXX+fSumYY;

    // compute eigenstuff, smallest eigenvalue is in last position
    kES.DecrSortEigenStuff3();

    // unit-length direction for best-fit line
    kES.GetEigenvector(2,kLine.Direction);

    return kLine;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
Line3<float> OrthogonalLineFit3<float> (int, const Vector3<float>*,
    const bool*);

// double
template WM3_ITEM
Line3<double> OrthogonalLineFit3<double> (int, const Vector3<double>*,
    const bool*);
//----------------------------------------------------------------------------
}
