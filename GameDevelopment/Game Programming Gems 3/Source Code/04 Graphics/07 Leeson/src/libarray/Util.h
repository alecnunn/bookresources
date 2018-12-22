/* Copyright (C) William Leeson, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William Leeson, 2001"
 */
/****************************************************************************\
 *                       Utility functions for Array Library                *
 ****************************************************************************
 * File: util.h                                                             *
 * Date: $Date: 2001/07/12 10:59:02 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.2 $                                                *
 * Description: This provides a set of functions to handle various          *
 *              misc operations on arrays.                                  *
\****************************************************************************/
#if HAVE_CONFIG_H
   #include <config.h>
#endif

#include "libarray.h"

#ifndef _UTIL_ARRAY_
#define _UTIL_ARRAY_
#ifdef __cplusplus
extern "C"
{
#endif
  real Periodic(real t,real min,real max);
  int FindIndex(real t,int n,real *array);
  real rad2deg(real rad);
  real deg2rad(real deg);
#ifdef __cplusplus
}
#endif
#endif
