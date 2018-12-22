// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRSPHERE3FRUSTUM3_H
#define WM3INTRSPHERE3FRUSTUM3_H

#include "Wm3Intersector.h"
#include "Wm3Sphere3.h"
#include "Wm3Frustum3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrSphere3Frustum3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrSphere3Frustum3 (const Sphere3<Real>& rkSphere,
        const Frustum3<Real>& rkFrustum);

    // object access
    const Sphere3<Real>& GetSphere () const;
    const Frustum3<Real>& GetFrustum () const;

    // static intersection query
    virtual bool Test ();

private:
    // the objects to intersect
    const Sphere3<Real>& m_rkSphere;
    const Frustum3<Real>& m_rkFrustum;
};

typedef IntrSphere3Frustum3<float> IntrSphere3Frustum3f;
typedef IntrSphere3Frustum3<double> IntrSphere3Frustum3d;

}

#endif
