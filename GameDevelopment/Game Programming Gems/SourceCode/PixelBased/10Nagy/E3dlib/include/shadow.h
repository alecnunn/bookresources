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
/* - Shadow library header file							*/
/*										*/
/* AUTHOR:	Gabor Nagy							*/
/* DATE:	2000-Feb-15 18:58:20						*/
/*  v1.0									*/
/*										*/
/* For Game Programming Graphics Gems						*/
/*==============================================================================*/
#include <math.h>
#include <stdlib.h>

#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
extern "C" {
#endif

extern void	Sh_CreateShadowMap(GLuint LShadowMapTexId, int LMapXSize, int LMapYSize, E3dMesh* LBlockerMesh, E3dMatrix LBlockerMatrix);

#if defined (__cplusplus) && (!(defined (__CPLUSLIBS)))
}	// Close scope of 'extern "C"' declaration which encloses file.
#endif
