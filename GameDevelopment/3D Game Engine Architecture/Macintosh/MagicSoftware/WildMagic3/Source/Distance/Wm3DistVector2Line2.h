// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3DISTVECTOR2LINE2_H
#define WM3DISTVECTOR2LINE2_H

#include "Wm3Distance.h"
#include "Wm3Line2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM DistVector2Line2 : public Distance<Real,Vector2<Real> >
{
public:
    DistVector2Line2 (const Vector2<Real>& rkVector,
        const Line2<Real>& rkLine);

    // object access
    const Vector2<Real>& GetVector () const;
    const Line2<Real>& GetLine () const;

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
    const Line2<Real>& m_rkLine;
};

typedef DistVector2Line2<float> DistVector2Line2f;
typedef DistVector2Line2<double> DistVector2Line2d;

}

#endif
