// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BOUNDINGVOLUME_H
#define WM3BOUNDINGVOLUME_H

#include "Wm3Object.h"
#include "Wm3Plane3.h"
#include "Wm3Ray3.h"
#include "Wm3Transformation.h"
#include "Wm3Vector3Array.h"

namespace Wm3
{

class WM3_ITEM BoundingVolume : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // abstract base class
    virtual ~BoundingVolume ();

    // run-time type information
    enum // BVType
    {
        BV_SPHERE,
        BV_BOX,
        BV_QUANTITY
    };
    virtual int GetBVType () const = 0;

    // all bounding volumes must define a center and radius
    virtual void SetCenter (const Vector3f& rkCenter) = 0;
    virtual void SetRadius (float fRadius) = 0;
    virtual Vector3f GetCenter () const = 0;
    virtual float GetRadius () const = 0;

    // One of the derived classes has the responsibility for implementing
    // this factory function.  Our default is the WmlSphereBV class, but
    // you may change it to another.
    static BoundingVolume* Create ();

    // Compute a bounding volume that contains all the points.
    virtual void ComputeFromData (const Vector3fArray* pkVertices) = 0;

    // Transform the bounding volume (model-to-world conversion).
    virtual void TransformBy (const Transformation& rkTransform,
        BoundingVolume* pkResult) = 0;

    // Determine if the bounding volume is one side of the plane, the other
    // side, or straddles the plane.  If it is on the positive side (the
    // side to which the normal points), the return value is +1.  If it is
    // on the negative side, the return value is -1.  If it straddles the
    // plane, the return value is 0.
    virtual int WhichSide (const Plane3f& rkPlane) const = 0;

    // Test for intersection of ray and bound (points of intersection not
    // computed).  The input direction must be unit length.
    virtual bool TestIntersection (const Ray3f& rkRay) const = 0;

    // Test for intersection of the two bounds.
    virtual bool TestIntersection (const BoundingVolume* pkInput) const = 0;

    // Make a copy of the bounding volume.
    virtual void CopyFrom (const BoundingVolume* pkInput) = 0;

    // Change the current bounding volume so that it contains the input
    // bounding volume as well as its old bounding volume.
    virtual void GrowToContain (const BoundingVolume* pkInput) = 0;

    // test for containment of a point
    virtual bool Contains (const Vector3f& rkPoint) const = 0;

protected:
    BoundingVolume ();
};

WM3_REGISTER_STREAM(BoundingVolume);
typedef Pointer<BoundingVolume> BoundingVolumePtr;

}

#endif
