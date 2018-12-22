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
 *                         Array Utility Function Library                   *
 ****************************************************************************
 * File: Util.c                                                             *
 * Date: $Date: 2001/07/27 10:04:36 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.3 $                                                *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on arrays.                             *
\****************************************************************************/
#include "Util.h"

/*
 *	Binary search through an array to find closest lower match
 */
int FindIndex(real t,int n,real *array)
{
 int h,l,k;

 /* binary search for time */
 h = n - 1;
 l = 0;
 while((h - l) > 1)
   {
     k = (h + l)/2;
     if(array[k] > t)
       {
	 h = k;
       }
     else
       {
	 l = k;
       }
   } 
 return l;
}

/*
 * Converts radians to degrees
 */
real rad2deg(real rad)
{
  return (rad*180.0)/M_PI;
}

/*
 * Converts degrees to radians
 */
real deg2rad(real deg)
{
  return (deg*M_PI)/180.0;
}

/*
 * periodic
 */
real Periodic(real t,real min,real max)
{
  /* printf("%f-<%f,%f>%f->",t,min,max,floor((t - min)/(max - min))); */
  t = t - (max - min)*(floor((t - min)/(max - min)));
  /* printf("[%f]",t); */

  return t;
}


