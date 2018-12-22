// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRRAY3LOZENGE3_H
#define WM3INTRRAY3LOZENGE3_H

#include "Wm3Intersector.h"
#include "Wm3Ray3.h"
#include "Wm3Lozenge3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrRay3Lozenge3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrRay3Lozenge3 (const Ray3<Real>& rkRay,
        const Lozenge3<Real>& rkLozenge);

    // object access
    const Ray3<Real>& GetRay () const;
    const Lozenge3<Real>& GetLozenge () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Ray3<Real>& m_rkRay;
    const Lozenge3<Real>& m_rkLozenge;
};

typedef IntrRay3Lozenge3<float> IntrRay3Lozenge3f;
typedef IntrRay3Lozenge3<double> IntrRay3Lozenge3d;

}

#endif
