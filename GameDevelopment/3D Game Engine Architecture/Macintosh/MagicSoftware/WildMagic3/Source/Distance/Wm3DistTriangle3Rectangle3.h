// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTTRIANGLE3RECTANGLE3_H
#define WM3DISTTRIANGLE3RECTANGLE3_H

#include "Wm3Distance.h"
#include "Wm3Triangle3.h"
#include "Wm3Rectangle3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistTriangle3Rectangle3 : public Distance<Real,Vector3<Real> >
{
public:
    DistTriangle3Rectangle3 (const Triangle3<Real>& rkTriangle,
        const Rectangle3<Real>& rkRectangle);

    // object access
    const Triangle3<Real>& GetTriangle () const;
    const Rectangle3<Real>& GetRectangle () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

private:
    const Triangle3<Real>& m_rkTriangle;
    const Rectangle3<Real>& m_rkRectangle;
};

typedef DistTriangle3Rectangle3<float> DistTriangle3Rectangle3f;
typedef DistTriangle3Rectangle3<double> DistTriangle3Rectangle3d;

}

#endif
