// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLINE3LOZENGE3_H
#define WM3INTRLINE3LOZENGE3_H

#include "Wm3Intersector.h"
#include "Wm3Line3.h"
#include "Wm3Lozenge3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLine3Lozenge3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLine3Lozenge3 (const Line3<Real>& rkLine,
        const Lozenge3<Real>& rkLozenge);

    // object access
    const Line3<Real>& GetLine () const;
    const Lozenge3<Real>& GetLozenge () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Line3<Real>& m_rkLine;
    const Lozenge3<Real>& m_rkLozenge;
};

typedef IntrLine3Lozenge3<float> IntrLine3Lozenge3f;
typedef IntrLine3Lozenge3<double> IntrLine3Lozenge3d;

}

#endif
