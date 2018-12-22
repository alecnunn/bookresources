/*============================================================================
	Title: RenderMan.h
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: Utility functions that are part of the render interface.
	Changes:
		25/01/2003 - Ignacio Castaño
			Math library started.

============================================================================*/

#ifndef _PI_RENDERMAN_H_
#define _PI_RENDERMAN_H_

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file RenderMan.h
 * Utility functions that are part of the render interface.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

//#include "Core.h"
//#include "MathLib.h"
#include "PAR.h"


/*----------------------------------------------------------------------------
	Functions:
----------------------------------------------------------------------------*/

/**
 * Renderman function 'step'. 
**/
inline REAL step( REAL a, REAL x ) {
	return (float)(x >= a);
}


/**
 * Renderman function 'linearstep'. 
**/
inline REAL linearstep( REAL a, REAL b, REAL x ) {
	if( x<a ) return 0.0;
	else if( x>b ) return 1.0;
	return (x-a)/(b-a);
}


/**
 * Renderman function 'smoothstep'. 
**/
inline REAL smoothstep( REAL a, REAL b, REAL x ) {
	if( x<a ) return 0.0;
	else if( x>b ) return 1.0;
	x = (x-a)/(b-a);
	return (x*x*(3-2*x));
}


/**
 * Renderman function 'clamp'. 
**/
inline REAL clamp( REAL x, REAL a, REAL b ) {
	return (x<a ? a : (x>b ? b : x));
}


/**
 * Renderman function 'mod'. 
**/
inline REAL mod( REAL a, REAL b ) {
	int n = (int)(a/b);
	a -= n*b;
	if( a<0 ) a+= b;
	return a;
}


/** 3d Perlin noise function. */
MATHLIB_API REAL noise3( REAL x, REAL y, REAL z );

/** 2d Perlin noise function. */
MATHLIB_API REAL noise2( REAL x, REAL y );

/** 1d Perlin noise function. */
MATHLIB_API REAL noise1( REAL x );




#endif // _PI_RENDERMAN_H_