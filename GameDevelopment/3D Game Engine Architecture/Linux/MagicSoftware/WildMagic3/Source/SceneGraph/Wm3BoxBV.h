// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BOXBV_H
#define WM3BOXBV_H

#include "Wm3BoundingVolume.h"
#include "Wm3Box3.h"

namespace Wm3
{

class WM3_ITEM BoxBV : public BoundingVolume
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // construction
    BoxBV ();  // center (0,0,0), axes (1,0,0),(0,1,0),(0,0,1), extents 1,1,1
    BoxBV (const Box3f& rkBox);

    virtual int GetBVType () const;

    // all bounding volumes must define a center and radius
    virtual void SetCenter (const Vector3f& rkCenter);
    virtual void SetRadius (float fRadius);
    virtual Vector3f GetCenter () const;
    virtual float GetRadius () const;

    Box3f& Box ();
    const Box3f& GetBox () const;

    // Compute a box that contains all the points.
    virtual void ComputeFromData (const Vector3fArray* pkVertices);

    // Transform the box (model-to-world conversion).
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

    // Change the current box so that it contains itself and the input.
    virtual void GrowToContain (const BoundingVolume* pkInput);

    // test for containment of a point
    virtual bool Contains (const Vector3f& rkPoint) const;

protected:
    Box3f m_kBox;
};

#include "Wm3BoxBV.inl"

WM3_REGISTER_STREAM(BoxBV);
typedef Pointer<BoxBV> BoxBVPtr;

}

#endif
