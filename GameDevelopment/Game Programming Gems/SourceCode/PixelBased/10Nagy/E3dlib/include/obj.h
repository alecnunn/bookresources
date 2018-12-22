/* Copyright (C) Gabor Nagy, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Gabor Nagy, 2000"
 */
/*==============================================================================*/
/* Real-time cast shadows							*/
/*										*/
/* - Simple Wavefront OBJ file reader include file				*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-Feb-22 14:58:07						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#include <math.h>
#include <stdlib.h>

#include "geo.h"

#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
extern "C" {
#endif

extern E3dModel*	E3d_ReadOBJFile(char* LFileName);

#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
}	// Close scope of 'extern "C"' declaration which encloses file.
#endif
