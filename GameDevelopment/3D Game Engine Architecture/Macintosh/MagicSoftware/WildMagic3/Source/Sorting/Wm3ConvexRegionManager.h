// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONVEXREGIONMANAGER_H
#define WM3CONVEXREGIONMANAGER_H

#include "Wm3BspNode.h"

namespace Wm3
{

class Camera;
class ConvexRegion;

class WM3_ITEM ConvexRegionManager : public BspNode
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction.  The BSP tree should be built so that the leaf nodes are
    // where the ConvexRegion objects are located.
    ConvexRegionManager ();

    // The middle child of ConvexRegionManager is where the representation
    // of the outside of the set of regions is stored.  This can be an
    // arbitrary subgraph, not just drawable geometry.
    SpatialPtr AttachOutside (Spatial* pkOutside);
    SpatialPtr DetachOutside ();
    SpatialPtr GetOutside ();

    // Determine region that contains the point.  If the point is outside
    // the set of regions, the return values is null.
    ConvexRegion* GetContainingRegion (const Vector3f& rkPoint);

protected:
    // drawing
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);
};

WM3_REGISTER_STREAM(ConvexRegionManager);
typedef Pointer<ConvexRegionManager> ConvexRegionManagerPtr;

}

#endif
