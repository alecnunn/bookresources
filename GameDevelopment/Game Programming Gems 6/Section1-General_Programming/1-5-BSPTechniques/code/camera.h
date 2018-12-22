/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Frustrum.h"

class Camera : public Ortho
{
public:
    Camera();
    Camera(V3& eye, REAL a = 0.0f, REAL e = 0.0f, REAL r = 0.0f);
    virtual ~Camera(){};
    void	PosCamera(V3& pos, V3& dir, V3& up){
        _wpos = pos;    
        _ldir = dir;
        _lup  = up;         // get to these
    }
    void	GetCamera(V3& pos, V3& dir, V3& up){
        pos = _wpos;
        dir = _ldir;
        up  = _lup;
    }
    const M4	GetViewMtx(REAL dTime);
    void	SetFar(REAL ffar){o_frust._farZ = ffar; }
    void	At(V3& v){_ldir = (v-_wpos).norm();}
    BOOL	IsCulled(V3* aabbMin, V3* aabbMax){return o_frust.IsCulled(aabbMin, aabbMax);}
    BOOL	IsCulled(V3* point){return o_frust.IsCulled(point);}
    REAL	GetFov(){return r_fov ;}
    void	SetFov(REAL fov){r_fov = fov;	}
    void    UpdateFrustrumPlanes(){o_frust.UpdateFrustrumPlanes(_wpos, _ldir, _lup, _lside); }
    
private:
    Frustrum            o_frust;
    REAL		        r_fov;           
};

#endif // !__CAMERA_H__
