// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ContCylinder3.h"
#include "Wm3ApprLineFit3.h"
#include "Wm3DistVector3Line3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Cylinder3<Real> ContCylinder (int iQuantity, const Vector3<Real>* akPoint)
{
    Cylinder3<Real> kCylinder;

    Line3<Real> kLine = OrthogonalLineFit3(iQuantity,akPoint);

    Real fMaxRadiusSqr = (Real)0.0;
    int i;
    for (i = 0; i < iQuantity; i++)
    {
        Real fRadiusSqr = DistVector3Line3<Real>(akPoint[i],
            kLine).GetSquared();
        if ( fRadiusSqr > fMaxRadiusSqr )
            fMaxRadiusSqr = fRadiusSqr;
    }

    Vector3<Real> kDiff = akPoint[0] - kLine.Origin;
    Real fWMin = kLine.Direction.Dot(kDiff), fWMax = fWMin;
    for (i = 1; i < iQuantity; i++)
    {
        kDiff = akPoint[i] - kLine.Origin;
        Real fW = kLine.Direction.Dot(kDiff);
        if ( fW < fWMin )
            fWMin = fW;
        else if ( fW > fWMax )
            fWMax = fW;
    }

    kCylinder.Segment.Origin = kLine.Origin +
        (((Real)0.5)*(fWMax+fWMin))*kLine.Direction;
    kCylinder.Segment.Direction = kLine.Direction;
    kCylinder.Radius = Math<Real>::Sqrt(fMaxRadiusSqr);
    kCylinder.Height = fWMax - fWMin;

    return kCylinder;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
Cylinder3<float> ContCylinder<float> (int, const Vector3<float>*);

// double
template WM3_ITEM
Cylinder3<double> ContCylinder<double> (int, const Vector3<double>*);
//----------------------------------------------------------------------------
}
