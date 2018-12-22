/* Copyright (C) Dante Treglia II, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dante Treglia II, 2000"
 */
#ifndef __DEF_H
#define __DEF_H

#ifdef WIN32
#include "win32.h"
#endif

typedef char			s8;
typedef unsigned char		u8;
typedef float			f32;
typedef double			f64;

#ifdef WIN32
typedef __int16			s16;
typedef __int32			s32;
typedef __int64			s64;
typedef unsigned __int16	u16;
typedef unsigned __int32	u32;
typedef unsigned __int64	u64;
#else
typedef signed short 		s16;
typedef signed long 		s32;
typedef signed long long 	s64;
typedef unsigned short		u16;
typedef unsigned long		u32;
typedef unsigned long long	u64;
#endif


typedef struct {u8 r, g, b, a;} COLOR;;
#define MALLOC malloc

#define SET_COLOR(c, rIn, gIn, bIn, aIn) \
	c.r = rIn; \
	c.g = gIn; \
	c.b = bIn; \
	c.a = aIn

#endif
