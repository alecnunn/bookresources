// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTCIRCLE3CIRCLE3_H
#define WM3DISTCIRCLE3CIRCLE3_H

#include "Wm3Distance.h"
#include "Wm3Circle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistCircle3Circle3 : public Distance<Real,Vector3<Real> >
{
public:
    DistCircle3Circle3 (const Circle3<Real>& rkCircle0,
        const Circle3<Real>& rkCircle1);

    // object access
    const Circle3<Real>& GetCircle0 () const;
    const Circle3<Real>& GetCircle1 () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Circle3<Real>& m_rkCircle0;
    const Circle3<Real>& m_rkCircle1;
};

typedef DistCircle3Circle3<float> DistCircle3Circle3f;
typedef DistCircle3Circle3<double> DistCircle3Circle3d;

}

#endif
