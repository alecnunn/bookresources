// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTVECTOR3ELLIPSOID3_H
#define WM3DISTVECTOR3ELLIPSOID3_H

#include "Wm3Distance.h"
#include "Wm3Ellipsoid3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistVector3Ellipsoid3 : public Distance<Real,Vector3<Real> >
{
public:
    DistVector3Ellipsoid3 (const Vector3<Real>& rkVector,
        const Ellipsoid3<Real>& rkEllipsoid);

    // object access
    const Vector3<Real>& GetVector () const;
    const Ellipsoid3<Real>& GetEllipsoid () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Vector3<Real>& m_rkVector;
    const Ellipsoid3<Real>& m_rkEllipsoid;
};

typedef DistVector3Ellipsoid3<float> DistVector3Ellipsoid32f;
typedef DistVector3Ellipsoid3<double> DistVector3Ellipsoid3d;

}

#endif
