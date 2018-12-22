/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#include "stdafx.h"
#include "baseos.h"
#include "basemath.h"
#include "csg_bsp.h"
#include "Scene.h" 
#include "z_ed3View.h"

//---------------------------------------------------------------------------------------
// Returns the total number of polygons in the scene
int	Scene::GetPolyCnt()
{
    int polys = 0;

    FOREACH(PBrushes, _primitives, pp)
    {
        Brush& pB = (*pp)->GetResult();;

        if(pB.IsSolid())
            polys+=pB._polys.size();
    }
    return polys;
}
