// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTVECTOR3CIRCLE3_H
#define WM3DISTVECTOR3CIRCLE3_H

#include "Wm3Distance.h"
#include "Wm3Circle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistVector3Circle3 : public Distance<Real,Vector3<Real> >
{
public:
    DistVector3Circle3 (const Vector3<Real>& rkVector,
        const Circle3<Real>& rkCircle);

    // object access
    const Vector3<Real>& GetVector () const;
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
    const Vector3<Real>& m_rkVector;
    const Circle3<Real>& m_rkCircle;
};

typedef DistVector3Circle3<float> DistVector3Circle3f;
typedef DistVector3Circle3<double> DistVector3Circle3d;

}

#endif
