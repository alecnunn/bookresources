/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/

#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "basemath.h"
#include "basegeom.h"
#include "Frustrum.h"

//---------------------------------------------------------------------------------------
// Camera class
class Camera : public OrtoPos
{
public:
    typedef enum _CAM_MODE{CAM_ONPOS, CAM_DAMP, CAM_BACKVIEW, CAM_UPVIEW, CAM_FRONTVIEW}CAM_MODE;
    Camera();
    Camera(V3& eye, REAL a = 0.0f, REAL e = 0.0f, REAL r = 0.0f);
    virtual     ~Camera(){};
    const M4&	GetViewMtx(REAL dTime);
    M4&			TrMatrix(){return _trmat;}
    void		SetFar(REAL ffar){_frustrum._farZ = ffar; }
    BOOL		IsCulled(V3* aabbMin, V3* aabbMax){return _frustrum.IsCulled(aabbMin, aabbMax);}
    void        UpdateFrustrumPlanes(){_frustrum.UpdateFrustrumPlanes(_wpos, _ldir, _lup, _lside); }
private:
    Frustrum            _frustrum;
    REAL		        _fov;           // Field of view
};


#endif // !__CAMERA_H__
