// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BOXBVTREE_H
#define WM3BOXBVTREE_H

#include "Wm3BoundingVolumeTree.h"
#include "Wm3BoxBV.h"

namespace Wm3
{

class WM3_ITEM BoxBVTree : public BoundingVolumeTree
{
    WM3_DECLARE_INITIALIZE;

public:
    BoxBVTree (const TriMesh* pkMesh, int iMaxTrisPerLeaf = 1,
        bool bStoreInteriorTris = false);

protected:
    static BoundingVolume* CreateModelBound (const TriMesh* pkMesh, int i0,
        int i1, int* aiISplit, Line3f& rkLine);

    static BoundingVolume* CreateWorldBound ();
};

WM3_REGISTER_INITIALIZE(BoxBVTree);

}

#endif
