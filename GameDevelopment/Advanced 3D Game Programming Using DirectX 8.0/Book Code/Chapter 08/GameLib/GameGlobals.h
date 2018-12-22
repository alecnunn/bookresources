/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: GameGlobals.h
 *    Desc: Some global defines we'll need.
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _GAMEGLOBALS_H
#define _GAMEGLOBALS_H

#define POWER_OF_2(x) (((x) & ((x)-1))? false : true)

#define DP( a )\
{\
	char buff[1024];\
	sprintf( buff, a );\
	OutputDebugString( buff );\
}

#define DP0 DP

#define DP1( a, b )\
{\
	char buff[1024];\
	sprintf( buff, a, b );\
	OutputDebugString( buff );\
}

#define DP2( a, b, c )\
{\
	char buff[1024];\
	sprintf( buff, a, b, c );\
	OutputDebugString( buff );\
}

#define DP3( a, b, c, d )\
{\
	char buff[1024];\
	sprintf( buff, a, b, c, d );\
	OutputDebugString( buff );\
}

#define DP4( a, b, c, d, e )\
{\
	char buff[1024];\
	sprintf( buff, a, b, c, d, e );\
	OutputDebugString( buff );\
}

/**
 * useful here and there, allows the user to 'snap' a variable to lie
 * within the bounds of two inputted vars
 */
template <class T>
static inline Snap( T &a, T min, T max )
{
	if( a < min ) 
		a = min;
	if( a > max ) 
		a = max;
}

inline float RandFloat( float min = 0.f, float max = 1.f )
{
	return min + (max-min)*((float)rand()/RAND_MAX);
}

#endif //_GAMEGLOBALS_H