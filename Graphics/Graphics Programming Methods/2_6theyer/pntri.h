/*****************************************************************
 * Copyright (c) 2002 TheyerGFX Pty Ltd (www.theyergfx.com)
 *****************************************************************
 * THE MATERIAL EMBODIED ON THIS SOFTWARE IS PROVIDED TO YOU "AS-IS"
 * AND WITHOUT WARRANTY OF ANY KIND, EXPRESS, IMPLIED OR OTHERWISE,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY OR
 * FITNESS FOR A PARTICULAR PURPOSE.
 *****************************************************************
 * Project		: Game Programming Methods
 *****************************************************************
 * File			: pntri.h
 * Language		: ANSI C
 * Author		: Mark Theyer
 * Created		: 13 Dec 2002
 *****************************************************************
 * Description	: API for Curved PN Triangles
 *****************************************************************/

#ifndef GAME_PROGRAMMING_METHODS_PNTRI_H
#define GAME_PROGRAMMING_METHODS_PNTRI_H

/*
 * includes
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * macros
 */

/*
 * typedefs
 */

typedef struct {
	float	x,y,z;
} Point_f3d;

/*
 * prototypes
 */

// standard PN triangle functions
extern void		pnControlNet( Point_f3d *p, Point_f3d *n, Point_f3d *out );
extern int		pnBezierPatch( Point_f3d *net, int lod, Point_f3d *pout, Point_f3d *nout );
extern int		pnTessellate( Point_f3d *p, int lod, short *tpts );
extern int		pnInterpolate( float *values, int nvalues, int lod, float *vout );

// enhanced PN triangle functions
extern void		pnControlNet3( Point_f3d *p, Point_f3d *n, Point_f3d *out );
extern int		pnBezierPatch3( Point_f3d *net, int nsides, int lod, Point_f3d *pout, Point_f3d *nout );
extern int		pnTessellate3( Point_f3d *p, int nsides, int lod, short *tpts );
extern int		pnInterpolate3( float *values, int nvalues, int nsides, int lod, float *vout );

// new PN quad' functions
extern void		pnControlNet4( Point_f3d *p, Point_f3d *n, Point_f3d *out );
extern int		pnBezierPatch4( Point_f3d *net, int nsides, int lod, Point_f3d *pout, Point_f3d *nout );
extern int		pnTessellate4( Point_f3d *p, int nsides, int lod, short *tpts );
extern int		pnInterpolate4( float *values, int nvalues, int nsides, int lod, float *vout );

#endif // GAME_PROGRAMMING_METHODS_PNTRI_H

