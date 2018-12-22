/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: MathGlobal.h
 *    Desc: a few global math functions
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _MATHGLOBAL_H
#define _MATHGLOBAL_H

#ifndef EPSILON
#define EPSILON 0.001f
#endif

#ifndef PI
#define PI 3.1415926f
#endif

#ifndef SIGN_BIT
#define SIGN_BIT 0x80000000
#endif

#ifndef INV_SIGN_BIT
#define INV_SIGN_BIT 0x7FFFFFFF
#endif

#include <stdlib.h>

// perform absolute value by AND'ing out the sign bit
inline float FastFabs( const float &in )
{
	int* temp = (int*)&in;
	int out = *temp & INV_SIGN_BIT;
	return *((float*)&out);
}

// generates a random number between -1 and 1
inline float FRand()
{
	return (float)(rand()-RAND_MAX/2) / (RAND_MAX/2);
}

#endif //_MATHGLOBAL_H