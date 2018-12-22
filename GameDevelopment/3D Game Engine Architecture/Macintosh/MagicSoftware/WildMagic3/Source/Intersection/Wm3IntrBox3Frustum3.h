// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRBOX3FRUSTUM3_H
#define WM3INTRBOX3FRUSTUM3_H

#include "Wm3Intersector.h"
#include "Wm3Box3.h"
#include "Wm3Frustum3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrBox3Frustum3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrBox3Frustum3 (const Box3<Real>& rkBox,
        const Frustum3<Real>& rkFrustum);

    // object access
    const Box3<Real>& GetBox () const;
    const Frustum3<Real>& GetFrustum () const;

    // test-intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Box3<Real>& m_rkBox;
    const Frustum3<Real>& m_rkFrustum;
};

typedef IntrBox3Frustum3<float> IntrBox3Frustum3f;
typedef IntrBox3Frustum3<double> IntrBox3Frustum3d;

}

#endif
