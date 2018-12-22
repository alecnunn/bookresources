// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Camera.h"
#include "Wm3ConvexRegionManager.h"
#include "Wm3ConvexRegion.h"
#include "Wm3Renderer.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ConvexRegionManager,BspNode);
WM3_IMPLEMENT_STREAM(ConvexRegionManager);
WM3_IMPLEMENT_DEFAULT_STREAM(ConvexRegionManager,BspNode);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ConvexRegionManager,BspNode);

//----------------------------------------------------------------------------
ConvexRegionManager::ConvexRegionManager ()
{
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::AttachOutside (Spatial* pkOutside)
{
    return SetChild(1,pkOutside);
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::DetachOutside ()
{
    return DetachChildAt(1);
}
//----------------------------------------------------------------------------
SpatialPtr ConvexRegionManager::GetOutside ()
{
    return GetChild(1);
}
//----------------------------------------------------------------------------
ConvexRegion* ConvexRegionManager::GetContainingRegion (
    const Vector3f& rkPoint)
{
    return DynamicCast<ConvexRegion>(GetContainingNode(rkPoint));
}
//----------------------------------------------------------------------------
void ConvexRegionManager::Draw (Renderer& rkRenderer, bool bNoCull)
{
    ConvexRegion* pkRegion = GetContainingRegion(
        rkRenderer.GetCamera()->GetWorldLocation());

    if ( pkRegion )
    {
        // inside the set of regions, start drawing with region of camera
        pkRegion->Draw(rkRenderer,bNoCull);
    }
    else
    {
        // outside the set of regions, draw the outside scene (if it exists)
        if ( GetOutside() )
            GetOutside()->Draw(rkRenderer,bNoCull);
    }
}
//----------------------------------------------------------------------------
