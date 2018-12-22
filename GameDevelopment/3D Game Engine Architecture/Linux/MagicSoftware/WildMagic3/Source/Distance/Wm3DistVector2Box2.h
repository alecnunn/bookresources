// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTVECTOR2BOX2_H
#define WM3DISTVECTOR2BOX2_H

#include "Wm3Distance.h"
#include "Wm3Box2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistVector2Box2 : public Distance<Real,Vector2<Real> >
{
public:
    DistVector2Box2 (const Vector2<Real>& rkVector, const Box2<Real>& rkBox);

    // object access
    const Vector2<Real>& GetVector () const;
    const Box2<Real>& GetBox () const;

    // static distance queries
    virtual Real Get ();
    virtual Real GetSquared ();

    // function calculations for dynamic distance queries
    virtual Real Get (Real fT, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);
    virtual Real GetSquared (Real fT, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);

private:
    const Vector2<Real>& m_rkVector;
    const Box2<Real>& m_rkBox;
};

typedef DistVector2Box2<float> DistVector2Box2f;
typedef DistVector2Box2<double> DistVector2Box2d;

}

#endif
