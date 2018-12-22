// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3PORTAL_H
#define WM3PORTAL_H

#include "Wm3Spatial.h"

namespace Wm3
{

class Camera;
class ConvexRegion;
class Renderer;

class WM3_ITEM Portal : public Object
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // The portal is a unidirectional connector between two regions.  The
    // vertices of the portal must satisfy the following constraints:
    // 1. They must form a planar convex polygon (quantity >= 3 is implied).
    // 2. They must be counterclockwise ordered when looking through the
    //    portal to the adjacent region.
    // 3. They must be in the model space coordinates for the region that
    //    contains the portal.

    // Construction and destruction.  Portal accepts responsibility for
    // deleting the input array.
    Portal (int iVQuantity, Vector3f* akModelVertex,
        ConvexRegion* pkAdjacentRegion, bool bOpen);

    virtual ~Portal ();

    // member access
    ConvexRegion*& AdjacentRegion ();
    bool& Open ();

protected:
    // streaming
    Portal ();

    // updates
    friend class ConvexRegion;
    void UpdateWorldData (const Transformation& rkWorld);

    // drawing
    void Draw (Renderer& rkRenderer);

    // portal vertices
    int m_iVQuantity;
    Vector3f* m_akModelVertex;
    Vector3f* m_akWorldVertex;

    // region to which portal leads
    ConvexRegion* m_pkAdjacentRegion;

    // portals can be open or closed
    bool m_bOpen;
};

WM3_REGISTER_STREAM(Portal);
typedef Pointer<Portal> PortalPtr;
#include "Wm3Portal.inl"

}

#endif
