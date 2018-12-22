/*
	Csg by Bsp.
	Author:			Octavian Chincisan, 
	General Programming, Game Programming Gems 5
*/
#include "stdafx.h"
#include "camera.h"

//--------------------------------------------------------------------------------------------
Camera::Camera(V3& pos, REAL yaw, REAL pich, REAL rol)
{
    SetAngles(yaw, pich, rol);
    _wpos         = pos;
    _fov		  = 64.00;              // 64 degrees (pow of 2)
}

//--------------------------------------------------------------------------------------------
Camera::Camera()
{
}

//--------------------------------------------------------------------------------------------
//    GetViewMtx
const M4& Camera::GetViewMtx(REAL dTime)
{
    _trmat = MLook(_wpos, _ldir, _lup, _lside);
    return _trmat;
}



