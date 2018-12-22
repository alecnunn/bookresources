/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#ifndef __FRUSTRUM__H__
#define __FRUSTRUM__H__
#include "types.h"

class Camera;
class Frustrum
{
public:
	Frustrum():_ihulls(0){};
	virtual ~Frustrum(){};
	void		AddFrustrumPlan(V3& a,V3& b,V3& c){_hulls[_ihulls++].CalcNormal(a,b,c);}
	void		AddFrustrumPlan(V3& norm, V3& ponpl){_hulls[_ihulls++].Set(norm,ponpl);}
	void		AddFrustrumPlan(Plane& plane){_hulls[_ihulls++]=plane;}
    BOOL        IsCulled(V3* point);
	BOOL		IsCulled(V3* aabbMin, V3* aabbMax);
    void        UpdateFrustrumPlanes(V3& pos, V3& ldir, V3& lup, V3& lside);
public:          
	Plane		_hulls[32];		// maxim clipping hulls
	int			_ihulls;		// number of hulls
	V3			_viewfdots[5];	// frustrum corners
	REAL		_farZ;			// far
};

#endif // __FRUSTRUM_H__
