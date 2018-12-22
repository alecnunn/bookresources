#ifndef _SPLINE_H
#define _SPLINE_H

#include "stdafx.h"
//#include <D3dutil.h>
#include "State.h"

//float spline(float x, int nknots, float *knots);
//D3DXVECTOR3 spline(float x, int nknots, D3DXVECTOR3 *knots);
State spline (float x, int nknots, State *knot);


#endif