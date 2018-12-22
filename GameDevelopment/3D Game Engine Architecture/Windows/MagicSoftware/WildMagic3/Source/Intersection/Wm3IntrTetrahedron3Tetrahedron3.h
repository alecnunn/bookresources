// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRTETRAHEDRON3TETRAHEDRON3_H
#define WM3INTRTETRAHEDRON3TETRAHEDRON3_H

#include "Wm3Intersector.h"
#include "Wm3Tetrahedron3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrTetrahedron3Tetrahedron3 :
    public Intersector<Real,Vector3<Real> >
{
public:
    IntrTetrahedron3Tetrahedron3 (const Tetrahedron3<Real>& rkTetrahedron0,
        const Tetrahedron3<Real>& rkTetrahedron1);

    // object access
    const Tetrahedron3<Real>& GetTetrahedron0 () const;
    const Tetrahedron3<Real>& GetTetrahedron1 () const;

    // static query
    virtual bool Find ();

    // information about the intersection set
    const TArray<Tetrahedron3<Real> >& GetIntersection () const;

private:
    static void SplitAndDecompose (Tetrahedron3<Real> kTetra,
        const Plane3<Real>& rkPlane, TArray<Tetrahedron3<Real> >& rkInside);

    // the objects to intersect
    const Tetrahedron3<Real>& m_rkTetrahedron0;
    const Tetrahedron3<Real>& m_rkTetrahedron1;

    TArray<Tetrahedron3<Real> > m_kIntersection;
};

typedef IntrTetrahedron3Tetrahedron3<float> IntrTetrahedron3Tetrahedron3f;
typedef IntrTetrahedron3Tetrahedron3<double> IntrTetrahedron3Tetrahedron3d;

}

#endif
