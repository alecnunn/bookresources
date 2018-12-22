// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTLINE3RAY3_H
#define WM3DISTLINE3RAY3_H

#include "Wm3Distance.h"
#include "Wm3Line3.h"
#include "Wm3Ray3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistLine3Ray3 : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Ray3 (const Line3<Real>& rkLine, const Ray3<Real>& rkRay);

    // object access
    const Line3<Real>& GetLine () const;
    const Ray3<Real>& GetRay () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Line3<Real>& m_rkLine;
    const Ray3<Real>& m_rkRay;
};

typedef DistLine3Ray3<float> DistLine3Ray3f;
typedef DistLine3Ray3<double> DistLine3Ray3d;

}

#endif
