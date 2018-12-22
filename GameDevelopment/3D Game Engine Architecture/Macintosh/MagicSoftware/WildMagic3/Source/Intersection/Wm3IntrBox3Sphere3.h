// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRBOX3SPHERE3_H
#define WM3INTRBOX3SPHERE3_H

#include "Wm3Intersector.h"
#include "Wm3Box3.h"
#include "Wm3Sphere3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrBox3Sphere3 : public Intersector<Real,Vector3<Real> >
{
public:
    IntrBox3Sphere3 (const Box3<Real>& rkBox,
        const Sphere3<Real>& rkSphere);

    // object access
    const Box3<Real>& GetBox () const;
    const Sphere3<Real>& GetSphere () const;

    // test-intersection query
    virtual bool Test ();

    // find-intersection query
    virtual bool Find (Real fTMax, const Vector3<Real>& rkVelocity0,
        const Vector3<Real>& rkVelocity1);

    // intersection set for dynamic find-intersection query
    const Vector3<Real>& GetContactPoint () const;

private:
    // supporting functions for dynamic Find function
    static Real GetVertexIntersection (Real fDx, Real fDy, Real fDz,
        Real fVx, Real fVy, Real fVz, Real fRSqr);

    static Real GetEdgeIntersection (Real fDx, Real fDz, Real fVx, Real fVz,
        Real fVSqr, Real fRSqr);

    int FindFaceRegionIntersection (Real fEx, Real fEy, Real fEz,
        Real fCx, Real fCy, Real fCz, Real fVx, Real fVy, Real fVz,
        Real& rfIx, Real& rfIy, Real& rfIz, bool bAboveFace);

    int FindJustEdgeIntersection (Real fCy, Real fEx, Real fEy,
        Real fEz, Real fDx, Real fDz, Real fVx, Real fVy, Real fVz,
        Real& rfIx, Real& rfIy, Real& rfIz);

    int FindEdgeRegionIntersection (Real fEx, Real fEy, Real fEz,
        Real fCx, Real fCy, Real fCz, Real fVx, Real fVy, Real fVz,
        Real& rfIx, Real& rfIy, Real& rfIz, bool bAboveEdge);

    int FindVertexRegionIntersection (Real fEx, Real fEy, Real fEz,
        Real fCx, Real fCy, Real fCz, Real fVx, Real fVy, Real fVz,
        Real& rfIx, Real& rfIy, Real& rfIz);

    // the objects to intersect
    const Box3<Real>& m_rkBox;
    const Sphere3<Real>& m_rkSphere;

    // point of intersection
    Vector3<Real> m_kContactPoint;
};

typedef IntrBox3Sphere3<float> IntrBox3Sphere3f;
typedef IntrBox3Sphere3<double> IntrBox3Sphere3d;

}

#endif
