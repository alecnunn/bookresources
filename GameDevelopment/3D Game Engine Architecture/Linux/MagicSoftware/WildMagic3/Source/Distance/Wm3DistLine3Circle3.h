// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTLINE3CIRCLE3_H
#define WM3DISTLINE3CIRCLE3_H

#include "Wm3Distance.h"
#include "Wm3Line3.h"
#include "Wm3Circle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistLine3Circle3 : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Circle3 (const Line3<Real>& rkLine,
        const Circle3<Real>& rkCircle);

    // object access
    const Line3<Real>& GetLine () const;
    const Circle3<Real>& GetCircle () const;

    // Static distance queries.  Compute the distance from the point P to the
    // circle.  When P is on the normal line C+t*N where C is the circle
    // center and N is the normal to the plane containing the circle, then
    // all circle points are equidistant from P.  In this case the returned
    // point is (infinity,infinity,infinity).
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Line3<Real>& m_rkLine;
    const Circle3<Real>& m_rkCircle;
};

typedef DistLine3Circle3<float> DistLine3Circle3f;
typedef DistLine3Circle3<double> DistLine3Circle3d;

}

#endif
