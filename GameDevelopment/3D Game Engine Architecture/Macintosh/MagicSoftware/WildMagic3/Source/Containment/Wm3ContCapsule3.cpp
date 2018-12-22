// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ContCapsule3.h"
#include "Wm3ApprLineFit3.h"
#include "Wm3DistVector3Line3.h"
#include "Wm3DistVector3Segment3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Capsule3<Real> ContCapsule (int iQuantity, const Vector3<Real>* akPoint,
    const bool* abValid)
{
    Capsule3<Real> kCapsule;

    Line3<Real> kLine = OrthogonalLineFit3<Real>(iQuantity,akPoint,abValid);
    if ( kLine.Direction == Vector3<Real>::ZERO )
    {
        kCapsule.Segment.Origin = Vector3<Real>::ZERO;
        kCapsule.Segment.Direction = Vector3<Real>::ZERO;
        kCapsule.Segment.Extent = (Real)0.0;
        kCapsule.Radius = (Real)0.0;
        return kCapsule;
    }

    Real fMaxRadiusSqr = (Real)0.0;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        if ( !abValid || abValid[i] )
        {
            Real fRadiusSqr = DistVector3Line3<Real>(akPoint[i],
                kLine).GetSquared();
            if ( fRadiusSqr > fMaxRadiusSqr )
                fMaxRadiusSqr = fRadiusSqr;
        }
    }

    Vector3<Real> kU, kV, kW = kLine.Direction;
    Vector3<Real>::GenerateOrthonormalBasis(kU,kV,kW,true);

    Real fMin = Math<Real>::MAX_REAL, fMax = -fMin;
    for (i = 0; i < iQuantity; i++)
    {
        if ( !abValid || abValid[i] )
        {
            Vector3<Real> kDiff = akPoint[i] - kLine.Origin;
            Real fU = kU.Dot(kDiff);
            Real fV = kV.Dot(kDiff);
            Real fW = kW.Dot(kDiff);
            Real fDiscr = fMaxRadiusSqr - (fU*fU + fV*fV);
            Real fRadical = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));

            Real fTest = fW + fRadical;
            if ( fTest < fMin )
                fMin = fTest;

            fTest = fW - fRadical;
            if ( fTest > fMax )
                fMax = fTest;
        }
    }

    kCapsule.Radius = Math<Real>::Sqrt(fMaxRadiusSqr);
    kCapsule.Segment.Origin = kLine.Origin +
        (((Real)0.5)*(fMin+fMax))*kLine.Direction;
    kCapsule.Segment.Direction = kLine.Direction;

    if ( fMax > fMin )
    {
        // container is a capsule
        kCapsule.Segment.Extent = ((Real)0.5)*(fMax - fMin);
    }
    else
    {
        // container is a sphere
        kCapsule.Segment.Extent = (Real)0.0;
    }

    return kCapsule;
}
//----------------------------------------------------------------------------
template <class Real>
bool InCapsule (const Vector3<Real>& rkPoint, const Capsule3<Real>& rkCapsule)
{
    Real fDistance = DistVector3Segment3<Real>(rkPoint,
        rkCapsule.Segment).Get();
    return fDistance <= rkCapsule.Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool InCapsule (const Sphere3<Real>& rkSphere,
    const Capsule3<Real>& rkCapsule)
{
    Real fRDiff = rkCapsule.Radius - rkSphere.Radius;
    if ( fRDiff >= (Real)0.0 )
    {
        Real fDistance = DistVector3Segment3<Real>(rkSphere.Center,
            rkCapsule.Segment).Get();
        return fDistance <= fRDiff;
    }
    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool InCapsule (const Capsule3<Real>& rkTestCapsule,
    const Capsule3<Real>& rkCapsule)
{
    Sphere3<Real> kSpherePosEnd(
        rkTestCapsule.Segment.GetPosEnd(),
        rkTestCapsule.Radius);
        
    Sphere3<Real> kSphereNegEnd(
        rkTestCapsule.Segment.GetNegEnd(),
        rkTestCapsule.Radius);

    return InCapsule<Real>(kSpherePosEnd,rkCapsule)
        && InCapsule<Real>(kSphereNegEnd,rkCapsule);
}
//----------------------------------------------------------------------------
template <class Real>
Capsule3<Real> MergeCapsules (const Capsule3<Real>& rkCapsule0,
    const Capsule3<Real>& rkCapsule1)
{
    if ( InCapsule<Real>(rkCapsule0,rkCapsule1) )
        return rkCapsule1;

    if ( InCapsule<Real>(rkCapsule1,rkCapsule0) )
        return rkCapsule0;

    const Vector3<Real>& rkP0 = rkCapsule0.Segment.Origin;
    const Vector3<Real>& rkP1 = rkCapsule1.Segment.Origin;
    const Vector3<Real>& rkD0 = rkCapsule0.Segment.Direction;
    const Vector3<Real>& rkD1 = rkCapsule1.Segment.Direction;

    // axis of final capsule
    Line3<Real> kLine;

    // axis center is average of input axis centers
    kLine.Origin = ((Real)0.5)*(rkP0 + rkP1);

    // axis unit direction is average of input axis unit directions
    Vector3<Real> kDirection0 = rkD0;
    Vector3<Real> kDirection1 = rkD1;
    kDirection0.Normalize();
    kDirection1.Normalize();
    Vector3<Real>& rkLineDir = kLine.Direction;
    if ( kDirection0.Dot(kDirection1) >= (Real)0.0 )
        rkLineDir = kDirection0 + kDirection1;
    else
        rkLineDir = kDirection0 - kDirection1;
    rkLineDir.Normalize();

    // Cylinder with axis 'kLine' must contain the spheres centered at the
    // end points of the input capsules.
    Vector3<Real> kPosEnd0 = rkCapsule0.Segment.GetPosEnd();
    Real fRadius = DistVector3Line3<Real>(kPosEnd0,kLine).Get() +
        rkCapsule0.Radius;

    Vector3<Real> kNegEnd0 = rkCapsule0.Segment.GetNegEnd();
    Real fTmp = DistVector3Line3<Real>(kNegEnd0,kLine).Get() +
        rkCapsule0.Radius;
    if ( fTmp > fRadius )
        fRadius = fTmp;

    Vector3<Real> kPosEnd1 = rkCapsule1.Segment.GetPosEnd();
    fTmp = DistVector3Line3<Real>(kPosEnd1,kLine).Get() + rkCapsule1.Radius;
    if ( fTmp > fRadius )
        fRadius = fTmp;

    Vector3<Real> kNegEnd1;
    fTmp = DistVector3Line3<Real>(kNegEnd1,kLine).Get() + rkCapsule1.Radius;
    if ( fTmp > fRadius )
        fRadius = fTmp;

    // process sphere <PosEnd0,r0>
    Real fRDiff = fRadius - rkCapsule0.Radius;
    Real fRDiffSqr = fRDiff*fRDiff;
    Vector3<Real> kDiff = kLine.Origin - kPosEnd0;
    Real fK0 = kDiff.SquaredLength() - fRDiffSqr;
    Real fK1 = kDiff.Dot(kLine.Direction);
    Real fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    Real fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    Real fTPos = fK1 - fRoot;
    Real fTNeg = fK1 + fRoot;

    // process sphere <NegEnd0,r0>
    kDiff = kLine.Origin - kNegEnd0;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction);
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    fTmp = fK1 - fRoot;
    if ( fTmp > fTPos )
        fTPos = fTmp;
    fTmp = fK1 + fRoot;
    if ( fTmp < fTNeg )
        fTNeg = fTmp;

    // process sphere <PosEnd1,r1>
    fRDiff = fRadius - rkCapsule1.Radius;
    fRDiffSqr = fRDiff*fRDiff;
    kDiff = kLine.Origin - kPosEnd1;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction);
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    fTmp = fK1 - fRoot;
    if ( fTmp > fTPos )
        fTPos = fTmp;
    fTmp = fK1 + fRoot;
    if ( fTmp < fTNeg )
        fTNeg = fTmp;

    // process sphere <NegEnd1,r1>
    kDiff = kLine.Origin - kNegEnd1;
    fK0 = kDiff.SquaredLength() - fRDiffSqr;
    fK1 = kDiff.Dot(kLine.Direction);
    fDiscr = fK1*fK1 - fK0;  // assert:  K1*K1-K0 >= 0
    fRoot = Math<Real>::Sqrt(Math<Real>::FAbs(fDiscr));
    fTmp = fK1 - fRoot;
    if ( fTmp > fTPos )
        fTPos = fTmp;
    fTmp = fK1 + fRoot;
    if ( fTmp < fTNeg )
        fTNeg = fTmp;

    Capsule3<Real> kCapsule;
    kCapsule.Radius = fRadius;
    kCapsule.Segment.Origin = kLine.Origin +
        ((Real)0.5)*(fTPos+fTNeg)*kLine.Direction;
    kCapsule.Segment.Direction = kLine.Direction;

    if ( fTPos > fTNeg )
    {
        // container is a capsule
        kCapsule.Segment.Extent = ((Real)0.5)*(fTPos - fTNeg);
    }
    else
    {
        // container is a sphere
        kCapsule.Segment.Extent = (Real)0.0;
    }

    return kCapsule;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
Capsule3<float> ContCapsule<float> (int, const Vector3<float>*,
    const bool*);

template WM3_ITEM
bool InCapsule<float> (const Vector3<float>&, const Capsule3<float>&);

template WM3_ITEM
bool InCapsule<float> (const Sphere3<float>&, const Capsule3<float>&);

template WM3_ITEM
bool InCapsule<float> (const Capsule3<float>&, const Capsule3<float>&);

template WM3_ITEM
Capsule3<float> MergeCapsules<float> (const Capsule3<float>&,
    const Capsule3<float>&);

// double
template WM3_ITEM
Capsule3<double> ContCapsule<double> (int, const Vector3<double>*,
    const bool*);

template WM3_ITEM
bool InCapsule<double> (const Vector3<double>&, const Capsule3<double>&);

template WM3_ITEM
bool InCapsule<double> (const Sphere3<double>&, const Capsule3<double>&);

template WM3_ITEM
bool InCapsule<double> (const Capsule3<double>&, const Capsule3<double>&);

template WM3_ITEM
Capsule3<double> MergeCapsules<double> (const Capsule3<double>&,
    const Capsule3<double>&);
//----------------------------------------------------------------------------
}
