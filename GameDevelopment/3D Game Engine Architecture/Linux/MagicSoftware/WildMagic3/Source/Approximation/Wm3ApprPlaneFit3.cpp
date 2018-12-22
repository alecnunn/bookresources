// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ApprPlaneFit3.h"
#include "Wm3Eigen.h"
#include "Wm3LinearSystem.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
bool HeightPlaneFit3 (int iQuantity, const Vector3<Real>* akPoint, Real& rfA,
    Real& rfB, Real& rfC, const bool* abValid)
{
    // You need at least three points to determine the plane.  Even so, if
    // the points are on a vertical plane, there is no least-squares fit in
    // the 'height' sense.  This will be trapped by the determinant of the
    // coefficient matrix.

    // compute sums for linear system
    Real fSumX = (Real)0.0, fSumY = (Real)0.0, fSumZ = (Real)0.0;
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumXZ = (Real)0.0;
    Real fSumYY = (Real)0.0, fSumYZ = (Real)0.0;
    int i;

    // compute sums for linear system
    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++)
        {
            fSumX += akPoint[i].X();
            fSumY += akPoint[i].Y();
            fSumZ += akPoint[i].Z();
            fSumXX += akPoint[i].X()*akPoint[i].X();
            fSumXY += akPoint[i].X()*akPoint[i].Y();
            fSumXZ += akPoint[i].X()*akPoint[i].Z();
            fSumYY += akPoint[i].Y()*akPoint[i].Y();
            fSumYZ += akPoint[i].Y()*akPoint[i].Z();
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
                fSumZ += akPoint[i].Z();
                fSumXX += akPoint[i].X()*akPoint[i].X();
                fSumXY += akPoint[i].X()*akPoint[i].Y();
                fSumXZ += akPoint[i].X()*akPoint[i].Z();
                fSumYY += akPoint[i].Y()*akPoint[i].Y();
                fSumYZ += akPoint[i].Y()*akPoint[i].Z();
                iValidQuantity++;
            }
        }
        if ( iValidQuantity < 3 )
        {
            rfA = Math<Real>::MAX_REAL;
            rfB = Math<Real>::MAX_REAL;
            rfC = Math<Real>::MAX_REAL;
            return false;
        }
        iQuantity = iValidQuantity;
    }

    Real aafA[3][3] =
    {
        {fSumXX, fSumXY, fSumX},
        {fSumXY, fSumYY, fSumY},
        {fSumX,  fSumY,  (Real)iQuantity}
    };

    Real afB[3] =
    {
        fSumXZ,
        fSumYZ,
        fSumZ
    };

    Real afX[3];

    bool bNonsingular = LinearSystem<Real>().Solve3(aafA,afB,afX);
    if ( bNonsingular )
    {
        rfA = afX[0];
        rfB = afX[1];
        rfC = afX[2];
    }
    else
    {
        rfA = Math<Real>::MAX_REAL;
        rfB = Math<Real>::MAX_REAL;
        rfC = Math<Real>::MAX_REAL;
    }

    return bNonsingular;
}
//----------------------------------------------------------------------------
template <class Real>
Plane3<Real> OrthogonalPlaneFit3 (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid)
{
    int i;
    Vector3<Real> kDiff;
    Real fInvQuantity;

    // compute the mean of the points
    Vector3<Real> kOrigin;
    if ( !abValid )
    {
        kOrigin = akPoint[0];
        for (i = 1; i < iQuantity; i++)
        {
            kOrigin += akPoint[i];
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
                kOrigin += akPoint[i];
                iValidQuantity++;
            }
        }
        if ( iValidQuantity == 0 )
            return Plane3<Real>(Vector3<Real>::ZERO,(Real)0.0);

        fInvQuantity = ((Real)1.0)/iValidQuantity;
    }
    kOrigin *= fInvQuantity;

    // compute sums of products
    Real fSumXX = (Real)0.0, fSumXY = (Real)0.0, fSumXZ = (Real)0.0;
    Real fSumYY = (Real)0.0, fSumYZ = (Real)0.0, fSumZZ = (Real)0.0;
    if ( !abValid )
    {
        for (i = 0; i < iQuantity; i++) 
        {
            kDiff = akPoint[i] - kOrigin;
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
                kDiff = akPoint[i] - kOrigin;
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

    // compute eigenstuff, smallest eigenvalue is in last position
    kES.DecrSortEigenStuff3();

    // get plane normal
    Vector3<Real> kNormal;
    kES.GetEigenvector(2,kNormal);

    // the minimum energy
    return Plane3<Real>(kNormal,kOrigin);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
bool HeightPlaneFit3<float> (int, const Vector3<float>*, float&, float&,
    float&, const bool*);

template WM3_ITEM
Plane3<float> OrthogonalPlaneFit3<float> (int, const Vector3<float>*,
    const bool*);

// double
template WM3_ITEM
bool HeightPlaneFit3<double> (int, const Vector3<double>*, double&, double&,
    double&, const bool*);

template WM3_ITEM
Plane3<double> OrthogonalPlaneFit3<double> (int, const Vector3<double>*,
    const bool*);
//----------------------------------------------------------------------------
}
