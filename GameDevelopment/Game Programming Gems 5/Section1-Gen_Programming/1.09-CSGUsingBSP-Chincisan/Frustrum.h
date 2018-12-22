/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __FRUSTRUM__H__
#define __FRUSTRUM__H__

//---------------------------------------------------------------------------------------
// Holds the frustrum planes and fov.
class Camera;
class Frustrum
{
public:
    Frustrum():_ihulls(0){};
    virtual ~Frustrum(){};

    void		AddFrustrumPlan(V3& a,V3& b,V3& c){
        _hulls[_ihulls++].CalcNormal(a,b,c);
    }
    void		AddFrustrumPlan(V3& norm, V3& ponpl){
        _hulls[_ihulls++].Set(norm,ponpl);
    }
    void		AddFrustrumPlan(Plane& plane){
        _hulls[_ihulls++]=plane;
    }
    BOOL		IsCulled(V3* aabbMin, V3* aabbMax);
    void        UpdateFrustrumPlanes(V3& pos, V3& ldir, V3& lup, V3& lside);

    Plane		_hulls[32];		// Maxim clipping hulls 
    int			_ihulls;		// number of hulls
    V3			_viewfdots[5];	// main frustrum corners
    REAL		_farZ;			// far

};

#endif // __FRUSTRUM_H__

