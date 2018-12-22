/*
    Game Programming Gems: Vol 6, General Programming
    Gem:    BSP Techniques
    Author: Octavian Marius Chincisan
*/
#include "stdafx.h"
#include "camera.h"

Camera::Camera(V3& pos, REAL yaw, REAL pich, REAL rol)
{
    SetAngles(yaw, pich, rol);
    _wpos         = pos;
    r_fov		  = 64;
}

Camera::Camera()
{
    r_fov    = 64;
}

const M4 Camera::GetViewMtx(REAL dTime)
{
    return MLook(_wpos, _ldir, _lup, _lside);
}



