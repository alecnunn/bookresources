/****************************************************************************\
 *                               Array Library                              *
 ****************************************************************************
 * File: libarray.h                                                         *
 * Date: $Date: 2001/11/08 12:06:10 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.5 $                                            *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on array type dattypes.                *
\****************************************************************************/

#ifndef _LIB_ARRAY_
#define _LIB_ARRAY_
/*
  #define X       0
  #define Y       1
  #define Z       2
  #define W       3
*/
#ifndef __cplusplus
#define bool int
#define true 1
#define false 0
#endif

#include <assert.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

/* define some of the types used in libarray */
#define real double
#define integer int
#define uinteger unsigned int
#define byte unsigned char
#define EPSILON 0.0000001
#ifndef M_PI
#define M_PI 3.1415926535897932384626433832795
#endif
typedef void (* eval_func)(real t,integer n,real *p_params,real *p_nodes,real *Q);

#include "libarray/Array2D.h"
#include "libarray/Array3D.h"
#include "libarray/Array4D.h"
#include "libarray/Matrix4x4.h"
#include "libarray/Quaternion.h"
#include "libarray/Matrix3x3.h"
#include "libarray/Util.h"
#endif


