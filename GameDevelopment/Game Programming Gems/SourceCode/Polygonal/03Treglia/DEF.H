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

typedef struct {unsigned char r, g, b, a;} COLOR;;

#include <win32.h>

#define SET_COLOR(c, rIn, gIn, bIn, aIn) \
	c.r = rIn; \
	c.g = gIn; \
	c.b = bIn; \
	c.a = aIn

#endif