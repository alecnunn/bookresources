// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3SPHEREBV_H
#define WM3SPHEREBV_H

#include "Wm3BoundingVolume.h"
#include "Wm3Sphere3.h"

namespace Wm3
{

class WM3_ITEM SphereBV : public BoundingVolume
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction
    SphereBV ();  // center (0,0,0), radius 0
    SphereBV (const Sphere3f& rkSphere);

    virtual int GetBVType () const;

    // all bounding volumes must define a center and radius
    virtual void SetCenter (const Vector3f& rkCenter);
    virtual void SetRadius (float fRadius);
    virtual Vector3f GetCenter () const;
    virtual float GetRadius () const;

    Sphere3f& Sphere ();
    const Sphere3f& GetSphere () const;

    // Compute a sphere that contains all the points.
    virtual void ComputeFromData (const Vector3fArray* pkVertices);

    // Transform the sphere (model-to-world conversion).
    virtual void TransformBy (const Transformation& rkTransform,
        BoundingVolume* pkResult);

    // Determine if the bounding volume is one side of the plane, the other
    // side, or straddles the plane.  If it is on the positive side (the
    // side to which the normal points), the return value is +1.  If it is
    // on the negative side, the return value is -1.  If it straddles the
    // plane, the return value is 0.
    virtual int WhichSide (const Plane3f& rkPlane) const;

    // Test for intersection of ray and bound (points of intersection not
    // computed).
    virtual bool TestIntersection (const Ray3f& rkRay) const;

    // Test for intersection of the two bounds.
    virtual bool TestIntersection (const BoundingVolume* pkInput) const;

    // Make a copy of the bounding volume.
    virtual void CopyFrom (const BoundingVolume* pkInput);

    // Change the current sphere so that it is the minimum volume sphere that
    // contains the input sphere as well as its old sphere.
    virtual void GrowToContain (const BoundingVolume* pkInput);

    // test for containment of a point
    virtual bool Contains (const Vector3f& rkPoint) const;

protected:
    Sphere3f m_kSphere;
};

#include "Wm3SphereBV.inl"

WM3_REGISTER_STREAM(SphereBV);
typedef Pointer<SphereBV> SphereBVPtr;

}

#endif
