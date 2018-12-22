// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ContEllipsoid3.h"
#include "Wm3ApprGaussPointsFit3.h"
#include "Wm3Quaternion.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Ellipsoid3<Real> ContEllipsoid (int iQuantity, const Vector3<Real>* akPoint)
{
    // Fit the points with a Gaussian distribution.  The covariance matrix
    // is M = D[0]*U[0]*U[0]^T+D[1]*U[1]*U[1]^T+D[2]*U[2]*U[2]^T where D[0],
    // D[1], and D[2] are the eigenvalues and U[0], U[1], and U[2] are
    // corresponding unit-length eigenvectors.
    Box3<Real> kBox = GaussPointsFit3(iQuantity,akPoint);
    Real* afD = kBox.Extent;
    Vector3<Real>* akAxis = kBox.Axis;

    // If either eigenvalue is nonpositive, adjust the D[] values so that
    // we actually build an ellipse.
    int i;
    for (i = 0; i < 3; i++)
    {
        if ( afD[i] < (Real)0.0 )
            afD[i] = -afD[i];
        if ( afD[i] < Math<Real>::ZERO_TOLERANCE )
            afD[i] = Math<Real>::ZERO_TOLERANCE;
    }

    // Grow the ellipsoid, while retaining its shape determined by the
    // covariance matrix, to enclose all the input points.  The quadratic form
    // that is used for the ellipsoid construction is
    //
    //   Q(X) = (X-C)^T*M*(X-C)
    //        = (X-C)^T*(sum_{j=0}^2 D[j]*U[j]*U[j]^T)*(X-C)
    //        = sum_{j=0}^2 D[j]*Dot(U[i],X-C)^2
    //
    // If the maximum value of Q(X[i]) for all input points is V^2, then a
    // bounding ellipsoid is Q(X) = V^2 since Q(X[i]) <= V^2 for all i.

    Real fMaxValue = (Real)0.0;
    for (i = 0; i < iQuantity; i++)
    {
        Vector3<Real> kDiff = akPoint[i] - kBox.Center;
        Real afDot[3] =
        {
            akAxis[0].Dot(kDiff),
            akAxis[1].Dot(kDiff),
            akAxis[2].Dot(kDiff)
        };

        Real fValue = afD[0]*afDot[0]*afDot[0] + afD[1]*afDot[1]*afDot[1] +
            afD[2]*afDot[2]*afDot[2];

        if ( fValue > fMaxValue )
            fMaxValue = fValue;
    }

    // arrange for quadratic to satisfy Q(X) <= 1
    Real fInv = ((Real)1.0)/fMaxValue;
    for (i = 0; i < 3; i++)
        afD[i] *= fInv;

    Ellipsoid3<Real> kEllipsoid;
    kEllipsoid.Center = kBox.Center;
    for (i = 0; i < 3; i++)
    {
        kEllipsoid.Axis[i] = akAxis[i];
        kEllipsoid.Extent[i] = Math<Real>::InvSqrt(afD[i]);
    }
    return kEllipsoid;
}
//----------------------------------------------------------------------------
template <class Real>
void ProjectEllipsoid (const Ellipsoid3<Real>& rkEllipsoid,
   const Line3<Real>& rkLine, Real& rfMin, Real& rfMax)
{
    // center of projection interval
    Real fCenter = rkLine.Direction.Dot(rkEllipsoid.Center - rkLine.Origin);

    // radius of projection interval
    Real afTmp[3] =
    {
        rkEllipsoid.Extent[0]*(rkLine.Direction.Dot(rkEllipsoid.Axis[0])),
        rkEllipsoid.Extent[1]*(rkLine.Direction.Dot(rkEllipsoid.Axis[1])),
        rkEllipsoid.Extent[2]*(rkLine.Direction.Dot(rkEllipsoid.Axis[2]))
    };
    Real fRSqr = afTmp[0]*afTmp[0] + afTmp[1]*afTmp[1] + afTmp[2]*afTmp[2];
    Real fRadius = Math<Real>::Sqrt(fRSqr);

    rfMin = fCenter - fRadius;
    rfMax = fCenter + fRadius;
}
//----------------------------------------------------------------------------
template <class Real>
const Ellipsoid3<Real> MergeEllipsoids (const Ellipsoid3<Real>& rkEllipsoid0,
    const Ellipsoid3<Real>& rkEllipsoid1)
{
    Ellipsoid3<Real> kMerge;

    // compute the average of the input centers
    kMerge.Center = ((Real)0.5)*(rkEllipsoid0.Center + rkEllipsoid1.Center);

    // bounding ellipsoid orientation is average of input orientations
    Quaternion<Real> kQ0(rkEllipsoid0.Axis), kQ1(rkEllipsoid1.Axis);
    if ( kQ0.Dot(kQ1) < (Real)0.0 )
        kQ1 = -kQ1;
    Quaternion<Real> kQ = kQ0+kQ1;
    kQ = Math<Real>::InvSqrt(kQ.Dot(kQ))*kQ;
    kQ.ToRotationMatrix(kMerge.Axis);

    // Project the input ellipsoids onto the axes obtained by the average
    // of the orientations and that go through the center obtained by the
    // average of the centers.
    for (int i = 0; i < 3; i++)
    {
        // projection axis
        Line3<Real> kLine(kMerge.Center,kMerge.Axis[i]);

        // project ellipsoids onto the axis
        Real fMin0, fMax0, fMin1, fMax1;
        ProjectEllipsoid(rkEllipsoid0,kLine,fMin0,fMax0);
        ProjectEllipsoid(rkEllipsoid1,kLine,fMin1,fMax1);

        // Determine the smallest interval containing the projected
        // intervals.
        Real fMax = ( fMax0 >= fMax1 ? fMax0 : fMax1 );
        Real fMin = ( fMin0 <= fMin1 ? fMin0 : fMin1 );

        // Update the average center to be the center of the bounding box
        // defined by the projected intervals.
        kMerge.Center += kLine.Direction*(((Real)0.5)*(fMin+fMax));

        // Compute the extents of the box based on the new center.
        kMerge.Extent[i] = ((Real)0.5)*(fMax-fMin);
    }

    return kMerge;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
Ellipsoid3<float> ContEllipsoid<float> (int, const Vector3<float>*);

template WM3_ITEM
void ProjectEllipsoid<float> (const Ellipsoid3<float>&,
    const Line3<float>&, float&, float&);

template WM3_ITEM
const Ellipsoid3<float> MergeEllipsoids<float> (const Ellipsoid3<float>&,
    const Ellipsoid3<float>&);

// double
template WM3_ITEM
Ellipsoid3<double> ContEllipsoid<double> (int, const Vector3<double>*);

template WM3_ITEM
void ProjectEllipsoid<double> (const Ellipsoid3<double>&,
    const Line3<double>&, double&, double&);

template WM3_ITEM
const Ellipsoid3<double> MergeEllipsoids<double> (const Ellipsoid3<double>&,
    const Ellipsoid3<double>&);
//----------------------------------------------------------------------------
}
