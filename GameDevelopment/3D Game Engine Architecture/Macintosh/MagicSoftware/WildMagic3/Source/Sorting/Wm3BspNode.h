// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BSPNODE_H
#define WM3BSPNODE_H

#include "Wm3Node.h"

namespace Wm3
{

class WM3_ITEM BspNode : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  The base class Node has *three* children
    // and is not allowed to grow.  The first and last children (indices 0 and
    // 2) are the "positive" and "negative" children of the binary tree.  The
    // positive child corresponds to the positive side of the separating
    // plane.  The negative child corresponds to the negative side of the
    // separating plane.  The middle child slot is where additional geometry
    // may be attached such as the triangles that are coplanar with the
    // separating plane.
    BspNode ();
    BspNode (const Plane3f& rkModelPlane);
    virtual ~BspNode ();

    // These methods should be used instead of the attach/detach methods in
    // the Node base class.
    SpatialPtr AttachPositiveChild (Spatial* pkChild);
    SpatialPtr AttachCoplanarChild (Spatial* pkChild);
    SpatialPtr AttachNegativeChild (Spatial* pkChild);
    SpatialPtr DetachPositiveChild ();
    SpatialPtr DetachCoplanarChild ();
    SpatialPtr DetachNegativeChild ();
    SpatialPtr GetPositiveChild ();
    SpatialPtr GetCoplanarChild ();
    SpatialPtr GetNegativeChild ();

    // plane access
    Plane3f& ModelPlane ();
    const Plane3f& GetModelPlane () const;
    const Plane3f& GetWorldPlane () const;

    // determine the portion of the scene that contains the point
    Spatial* GetContainingNode (const Vector3f& rkPoint);

protected:
    // geometric updates
    virtual void UpdateWorldData (double dAppTime);

    // drawing
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);

    Plane3f m_kModelPlane;
    Plane3f m_kWorldPlane;
};

WM3_REGISTER_STREAM(BspNode);
typedef Pointer<BspNode> BspNodePtr;
#include "Wm3BspNode.inl"

}

#endif
