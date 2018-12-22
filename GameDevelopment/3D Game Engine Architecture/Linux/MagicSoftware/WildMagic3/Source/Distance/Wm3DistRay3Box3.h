// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTRAY3BOX3_H
#define WM3DISTRAY3BOX3_H

#include "Wm3Distance.h"
#include "Wm3Ray3.h"
#include "Wm3Box3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistRay3Box3 : public Distance<Real,Vector3<Real> >
{
public:
    DistRay3Box3 (const Ray3<Real>& rkRay, const Box3<Real>& rkBox);

    // object access
    const Ray3<Real>& GetRay () const;
    const Box3<Real>& GetBox () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Ray3<Real>& m_rkRay;
    const Box3<Real>& m_rkBox;
};

typedef DistRay3Box3<float> DistRay3Box3f;
typedef DistRay3Box3<double> DistRay3Box3d;

}

#endif
