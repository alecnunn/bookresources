/* Copyright (C) Dan Ginsburg, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Dan Ginsburg, 2000"
 */
#ifndef _TRIBOX_H_
#define _TRIBOX_H_

///
//	Dependent Includes
//
#include "mtxlib.h"

///
//	Macros
//
#define INSIDE		0
#define OUTSIDE		1

#ifndef FALSE
	#define FALSE 0
#endif
#ifndef TRUE
	#define TRUE 1
#endif

///
//	Types
//
typedef struct
{
	vector3 Pt[3];

} TRI;

typedef struct
{
	vector3 Min,
			Max;
} BBOX;

///
//	Public Functions
//
int TriBoxIntersect(BBOX BBox, TRI Tri);

#endif