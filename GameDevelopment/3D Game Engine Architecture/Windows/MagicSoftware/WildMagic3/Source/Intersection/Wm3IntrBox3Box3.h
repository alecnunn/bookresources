// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRBOX3BOX3_H
#define WM3INTRBOX3BOX3_H

#include "Wm3Intersector.h"
#include "Wm3Box3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrBox3Box3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrBox3Box3 (const Box3<Real>& rkBox0, const Box3<Real>& rkBox1);

    // object access
    const Box3<Real>& GetBox0 () const;
    const Box3<Real>& GetBox1 () const;

    // static test-intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Box3<Real>& m_rkBox0;
    const Box3<Real>& m_rkBox1;
};

typedef IntrBox3Box3<float> IntrBox3Box3f;
typedef IntrBox3Box3<double> IntrBox3Box3d;

}

#endif
