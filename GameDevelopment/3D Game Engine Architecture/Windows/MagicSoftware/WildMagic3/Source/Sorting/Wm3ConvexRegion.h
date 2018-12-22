// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CONVEXREGION_H
#define WM3CONVEXREGION_H

#include "Wm3Node.h"

namespace Wm3
{

class Portal;

class WM3_ITEM ConvexRegion : public Node
{
    WM3_DECLARE_RTTI;
    WM3_DECLARE_NAME_ID;
    WM3_DECLARE_STREAM;

public:
    // Construction and destruction.  ConvexRegion accepts responsibility
    // for deleting the input array.
    ConvexRegion (int iPQuantity, Portal** apkPortal);
    virtual ~ConvexRegion ();

    // portal access
    int GetPortalQuantity () const;
    Portal* GetPortal (int i) const;

protected:
    // streaming
    ConvexRegion ();

    // geometric updates
    virtual void UpdateWorldData (double dAppTime);

    // portals of the region (these are not set up to be shared)
    int m_iPQuantity;
    Portal** m_apkPortal;

    // for region graph traversal
    bool m_bVisited;

// internal use
public:
    // Drawing.  ConvexRegionManager starts the region graph traversal
    // with the region containing the eye point.  Portal continues the
    // traversal.  Should you decide not to use a convex region manager and
    // track which region the eyepoint is in using your own system, you will
    // need to make sure that convex region that contains the eyepoint is
    // the first one visited during a depth-first traversal of the scene.
    virtual void Draw (Renderer& rkRenderer, bool bNoCull = false);
};

WM3_REGISTER_STREAM(ConvexRegion);
typedef Pointer<ConvexRegion> ConvexRegionPtr;
#include "Wm3ConvexRegion.inl"

}

#endif
