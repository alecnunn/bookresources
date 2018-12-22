// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRLOZENGE3LOZENGE3_H
#define WM3INTRLOZENGE3LOZENGE3_H

#include "Wm3Intersector.h"
#include "Wm3Lozenge3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrLozenge3Lozenge3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrLozenge3Lozenge3 (const Lozenge3<Real>& rkLozenge0,
        const Lozenge3<Real>& rkLozenge1);

    // object access
    const Lozenge3<Real>& GetLozenge0 () const;
    const Lozenge3<Real>& GetLozenge1 () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Lozenge3<Real>& m_rkLozenge0;
    const Lozenge3<Real>& m_rkLozenge1;
};

typedef IntrLozenge3Lozenge3<float> IntrLozenge3Lozenge3f;
typedef IntrLozenge3Lozenge3<double> IntrLozenge3Lozenge3d;

}

#endif
