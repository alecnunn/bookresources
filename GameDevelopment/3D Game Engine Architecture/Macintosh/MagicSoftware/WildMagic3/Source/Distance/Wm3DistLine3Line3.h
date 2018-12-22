// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTLINE3LINE3_H
#define WM3DISTLINE3LINE3_H

#include "Wm3Distance.h"
#include "Wm3Line3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistLine3Line3 : public Distance<Real,Vector3<Real> >
{
public:
    DistLine3Line3 (const Line3<Real>& rkLine0, const Line3<Real>& rkLine1);

    // object access
    const Line3<Real>& GetLine0 () const;
    const Line3<Real>& GetLine1 () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Line3<Real>& m_rkLine0;
    const Line3<Real>& m_rkLine1;
};

typedef DistLine3Line3<float> DistLine3Line3f;
typedef DistLine3Line3<double> DistLine3Line3d;

}

#endif
