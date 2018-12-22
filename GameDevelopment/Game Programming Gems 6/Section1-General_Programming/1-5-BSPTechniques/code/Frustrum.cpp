/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#include "stdafx.h"
#include "camera.h"
#include "frustrum.h"

void Frustrum::UpdateFrustrumPlanes(V3& pos, V3& ldir, V3& lup, V3& lside)
{
    _ihulls=0;
    _viewfdots[0] = pos;
        
    REAL heightFar = _farZ * 0.6713f;  // this goes with 640/480 and 60 degrees
    REAL widthFar  = _farZ * 0.8820f;   
    
    REAL tfx = _farZ * ldir.x;
    REAL tfy = _farZ * ldir.y;
    REAL tfz = _farZ * ldir.z;
    REAL wfx = widthFar*lside.x;
    REAL wfy = widthFar*lside.y;
    REAL wfz = widthFar*lside.z;
    REAL hfx = heightFar*lup.x;
    REAL hfy = heightFar*lup.y;
    REAL hfz = heightFar*lup.z;
    
    _viewfdots[4].x = pos.x + tfx + wfx + hfx;
    _viewfdots[4].y = pos.y + tfy + wfy + hfy;
    _viewfdots[4].z = pos.z + tfz + wfz + hfz;
    _viewfdots[3].x = pos.x + tfx + wfx - hfx;
    _viewfdots[3].y = pos.y + tfy + wfy - hfy;
    _viewfdots[3].z = pos.z + tfz + wfz - hfz;
    _viewfdots[2].x = pos.x + tfx - wfx - hfx;
    _viewfdots[2].y = pos.y + tfy - wfy - hfy;
    _viewfdots[2].z = pos.z + tfz - wfz - hfz;
    _viewfdots[1].x = pos.x + tfx - wfx + hfx;
    _viewfdots[1].y = pos.y + tfy - wfy + hfy;
    _viewfdots[1].z = pos.z + tfz - wfz + hfz;
    
    AddFrustrumPlan(ldir, pos);				                    // near
    AddFrustrumPlan(_viewfdots[1],_viewfdots[2],_viewfdots[3]); // far
    AddFrustrumPlan(_viewfdots[1],_viewfdots[0],_viewfdots[2]); // left
    AddFrustrumPlan(_viewfdots[4],_viewfdots[3],_viewfdots[0]); // right
    AddFrustrumPlan(_viewfdots[3],_viewfdots[2],_viewfdots[0]); // down
    AddFrustrumPlan(_viewfdots[1],_viewfdots[4],_viewfdots[0]); // up
}

BOOL    Frustrum::IsCulled(V3* am)
{
    for(int i=0; i<6; i++){
        if(_hulls[i].DistTo(am->x,am->y,am->z) > 0) continue;
        return TRUE;
    }
    return FALSE;
}

BOOL	Frustrum::IsCulled(V3* am, V3* aM)
{
    for(int i=0; i<6; i++){
        if(_hulls[i].DistTo(am->x,am->y,am->z) > 0) continue;
        if(_hulls[i].DistTo(aM->x,am->y,am->z) > 0) continue;
        if(_hulls[i].DistTo(am->x,aM->y,am->z) > 0) continue;
        if(_hulls[i].DistTo(aM->x,aM->y,am->z) > 0) continue;
        if(_hulls[i].DistTo(am->x,am->y,aM->z) > 0) continue;
        if(_hulls[i].DistTo(aM->x,am->y,aM->z) > 0) continue;
        if(_hulls[i].DistTo(am->x,aM->y,aM->z) > 0) continue;
        if(_hulls[i].DistTo(aM->x,aM->y,aM->z) > 0) continue;
        return TRUE;
    }
    return FALSE;
}
