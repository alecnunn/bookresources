// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprLineFit2.h"
#include "Wm3Eigen.h"
#include "Wm3LinearSystem.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
bool HeightLineFit2 (int iQuantity, const Vector2<Real>* akPoint, Real& rfA,
    Real& rfB, const bool* abValid)
{
    // You need at least two points to determine the line.  Even so, if
    // the points are on a vertical line, there is no least-squares fit in
    // the 'height' sense.  This will be trapped by the determinant of the
    // coefficient matrix being (nearly) zero.

    Real fSumX = (Real)0.0, fSumY = (Real)0.0;
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0;
    int i;

    // compute sums for linear system
    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++)
        {
            fSumX += akPoint[i].X();
            fSumY += akPoint[i].Y();
            fSumXX += akPoint[i].X()*akPoint[i].X();
            fSumXY += akPoint[i].X()*akPoint[i].Y();
        }
    }
    else
    {
        int iValidQuantity = 0;
        for (i = 0; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                fSumX += akPoint[i].X();
                fSumY += akPoint[i].Y();
                fSumXX += akPoint[i].X()*akPoint[i].X();
                fSumXY += akPoint[i].X()*akPoint[i].Y();
                iValidQuantity++;
            }
        }
        if ( iValidQuantity < 2 )
        {
            rfA = Math<Real>::MAX_REAL;
            rfB = Math<Real>::MAX_REAL;
            return false;
        }
        iQuantity = iValidQuantity;
    }

    Real aafA[2][2] =
    {
        {fSumXX, fSumX},
        {fSumX, (Real)iQuantity}
    };

    Real afB[2] =
    {
        fSumXY,
        fSumY
    };

    Real afX[2];

    bool bNonsingular = LinearSystem<Real>().Solve2(aafA,afB,afX);
    if ( bNonsingular )
    {
        rfA = afX[0];
        rfB = afX[1];
    }
    else
    {
        rfA = Math<Real>::MAX_REAL;
        rfB = Math<Real>::MAX_REAL;
    }

    return bNonsingular;
}
//----------------------------------------------------------------------------
template <class Real>
Line2<Real> OrthogonalLineFit2 (int iQuantity, const Vector2<Real>* akPoint,
    const bool* abValid)
{
    Line2<Real> kLine(Vector2<Real>::ZERO,Vector2<Real>::ZERO);
    int i;
    Vector2<Real> kDiff;
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
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumYY = (Real)0.0;
    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++) 
        {
            kDiff = akPoint[i] - kLine.Origin;
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
                kDiff = akPoint[i] - kLine.Origin;
                fSumXX += kDiff.X()*kDiff.X();
                fSumXY += kDiff.X()*kDiff.Y();
                fSumYY += kDiff.Y()*kDiff.Y();
            }
        }
    }

    fSumXX *= fInvQuantity;
    fSumXY *= fInvQuantity;
    fSumYY *= fInvQuantity;

    // set up the eigensolver
    Eigen<Real> kES(2);
    kES(0,0) = fSumYY;
    kES(0,1) = -fSumXY;
    kES(1,0) = -fSumXY;
    kES(1,1) = fSumXX;

    // compute eigenstuff, smallest eigenvalue is in last position
    kES.DecrSortEigenStuff2();

    // unit-length direction for best-fit line
    kES.GetEigenvector(1,kLine.Direction);

    return kLine;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
bool HeightLineFit2<float> (int, const Vector2<float>*, float&, float&,
    const bool*);

template WM3_ITEM
Line2<float> OrthogonalLineFit2<float> (int, const Vector2<float>*,
    const bool*);

// double
template WM3_ITEM
bool HeightLineFit2<double> (int, const Vector2<double>*, double&, double&,
    const bool*);

template WM3_ITEM
Line2<double> OrthogonalLineFit2<double> (int, const Vector2<double>*,
    const bool*);
//----------------------------------------------------------------------------
}
