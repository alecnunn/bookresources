/* Copyright (C) Steven Woodcock, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Steven Woodcock, 2001"
 */
//*********************************************************************
// Name:     util.h
// Purpose:  Various utilities and inline functions.
//*********************************************************************

#ifndef _UTIL_H
#define _UTIL_H

//
// includes
//
#include "mtxlib.h"

#include <math.h>
#include <stdlib.h>

///////////////////
// inline functions
///////////////////

// random number generator

#define RAND() (float) (rand()/(RAND_MAX * 1.0))
#define SIGN(x) (((x) < 0) ? -1 : 1)

////////////////////
// general utilities
////////////////////

// my own printf function; dumps prints to a file on the Desktop

extern "C" int myprintf( char *str, ...);   // my own version of printf

#endif