// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRBOX2BOX2_H
#define WM3INTRBOX2BOX2_H

#include "Wm3Intersector.h"
#include "Wm3Box2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrBox2Box2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrBox2Box2 (const Box2<Real>& rkBox0, const Box2<Real>& rkBox1);

    // object access
    const Box2<Real>& GetBox0 () const;
    const Box2<Real>& GetBox1 () const;

    // static test-intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Box2<Real>& m_rkBox0;
    const Box2<Real>& m_rkBox1;
};

typedef IntrBox2Box2<float> IntrBox2Box2f;
typedef IntrBox2Box2<double> IntrBox2Box2d;

}

#endif
