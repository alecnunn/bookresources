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
 *                            4D Array Function Library                     *
 ****************************************************************************
 * File: Array4D.c
 * Date: $Date: 2002/09/14 17:53:43 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This provides a set of functions to handle various
 *              operations performed on 4D array.
 *
 * Log: $log$
 */
#include "Array4D.h"

/*
 * Zero a vector
 */
void AZero4D(real *op)
{
  op[0] = 0.0;
  op[1] = 0.0;
  op[2] = 0.0;
  op[3] = 0.0;
}

/*
 * Copy
 */
void ACopy4D(const real *src,real *dest)
{
  dest[0] = src[0];
  dest[1] = src[1];
  dest[2] = src[2];
  dest[3] = src[3];
}

/*
 * Compare
 */
bool ACompare4D(const real *op1,const real *op2)
{

  return ((fabs(op1[0] - op2[0]) < EPSILON) && (fabs(op1[1] - op2[1]) < EPSILON) && (fabs(op1[2] - op2[2]) < EPSILON) && (fabs(op1[3] - op2[3]) < EPSILON));
}

/*
 * Add
 */
void AAdd4D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] + op2[0];
  result[1] = op1[1] + op2[1];
  result[2] = op1[2] + op2[2];
  result[3] = op1[3] + op2[3];
}

/*
 * Subtract
 */
void ASubtract4D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] - op2[0];
  result[1] = op1[1] - op2[1];
  result[2] = op1[2] - op2[2];
  result[3] = op1[3] - op2[3];
}

/*
 * Array Multiplication by a Scalar
 */
void ASMultiply4D(real op1,const real *op2,real *result)
{
  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
  result[2] = op1*op2[2];
  result[3] = op1*op2[3];
}

/*
 * Array Division by a Scalar
 */
void ASDivide4D(real op1,const real *op2,real *result)
{
  op1 = 1.0/op1;
  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
  result[2] = op1*op2[2];
  result[3] = op1*op2[3];
}

/*
 * Array Negation
 */
void ANegate4D(const real *op,real *result)
{
  result[0] = -op[0];
  result[1] = -op[1];
  result[2] = -op[2];
  result[3] = -op[3];
}

/*
 * Array by a Matrix
 */
void AMMultiply4D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[1]*op2[4] + op1[2]*op2[8]   + op1[3]*op2[12];
  result[1] = op1[0]*op2[1] + op1[1]*op2[5] + op1[2]*op2[9]   + op1[3]*op2[13];
  result[2] = op1[0]*op2[2] + op1[1]*op2[6] + op1[2]*op2[10]  + op1[3]*op2[14];
  result[4] = op1[0]*op2[3] + op1[1]*op2[7] + op1[2]*op2[11]  + op1[3]*op2[15];
}

/*
 *      Print 4D vector
 */
void APrint4D(const real *op)
{
  fprintf(stderr,"[%f %f %f %f]",op[0],op[1],op[2],op[3]);
}

/*
 *Determine min elements
 */
void AMinElements4D(const real *op1,const real *op2,real *result)
{
  result[0] = (op1[0] < op2[0]) ? op1[0] : op2[0];
  result[1] = (op1[1] < op2[1]) ? op1[1] : op2[1];
  result[2] = (op1[2] < op2[2]) ? op1[2] : op2[2];
  result[3] = (op1[3] < op2[3]) ? op1[3] : op2[3];
}

/*
 *Determine max elements
 */
void AMaxElements4D(const real *op1,const real *op2,real *result)
{
  result[0] = (op1[0] > op2[0]) ? op1[0] : op2[0];
  result[1] = (op1[1] > op2[1]) ? op1[1] : op2[1];
  result[2] = (op1[2] > op2[2]) ? op1[2] : op2[2];
  result[3] = (op1[3] > op2[3]) ? op1[3] : op2[3];
}

/*
 * Determine the magnitude of a 4D value
 */
real AMagnitude4D(const real *op)
{
  real d;

  d = op[0]*op[0] + op[1]*op[1] + op[2]*op[2] + op[3]*op[3];

  return sqrt(d);
}

/*
 * Determine the dotproduct of two vectors
 */
real ADotProduct4D(const real *op1,const real *op2)
{
  return op1[0]*op2[0] + op1[1]*op2[1] + op1[2]*op1[2] + op1[3]*op1[3];
}

/*
 * Produce unit lenght vector pointing in same direction
 */
void ANormalise4D(const real *op,real *result)
{
  real t;

  t = op[0]*op[0] + op[1]*op[1] + op[2]*op[2] + op[3]*op[3];
  t = 1.0/t;
  result[0] = op[0]*t;
  result[1] = op[1]*t;
  result[2] = op[1]*t;
  result[3] = op[2]*t;
}

/*
 *  divide across by the misterious h coordinate
 */
void AHomogenise4D(const real *op,real *result)
{
  real f = 1.0/op[3];

  result[0] = f*op[0];
  result[1] = f*op[1];
  result[2] = f*op[2];
  result[3] = 1.0;
}

/*
 * CrossProduct
 */
void ACrossProduct4D(const real *op1,const real *op2,const real *op3,real *result)
{
  result[0] = op1[1]*(op2[2]*op3[3] - op2[3]*op3[2])
    - op1[2]*(op2[1]*op3[3] - op2[3]*op3[1]) 
    + op1[3]*(op2[1]*op3[2] - op2[2]*op3[1]);

  result[1] =  op1[0]*(op2[2]*op3[3] - op2[3]*op3[2])
    - op1[2]*(op2[0]*op3[3] - op2[3]*op3[0]) 
    + op1[3]*(op2[0]*op3[2] - op2[2]*op3[0]);

  result[2] = op1[0]*(op2[1]*op3[3] - op2[3]*op3[1])
    - op1[1]*(op2[0]*op3[3] - op2[3]*op3[0]) 
    + op1[3]*(op2[0]*op3[1] - op2[1]*op3[0]);

  result[3] = op1[0]*(op2[1]*op3[2] - op2[2]*op3[1])
    - op1[1]*(op2[0]*op3[2] - op2[2]*op3[0]) 
    + op1[2]*(op2[0]*op3[1] - op2[1]*op3[0]);
}

/*
 * ALerp4D - linearly interpolates between two vectors
 */
void ALerp4D(real *op1,real *op2,real t,real *result)
{
  result[0] = t*op1[0] + (1 - t)*op2[0];
  result[1] = t*op1[1] + (1 - t)*op2[1];
  result[2] = t*op1[2] + (1 - t)*op2[2];
  result[3] = t*op1[3] + (1 - t)*op2[3];
}

/*
 * Projects the coordinate (op) to infinity in homogenious space from a point (pt)
 */
void Project2Infinity(real *op,real *pt,real *result)
{
  result[0] = op[0]*pt[3] - pt[0]*op[3];
  result[1] = op[1]*pt[3] - pt[1]*op[3];
  result[2] = op[2]*pt[3] - pt[2]*op[3];
  result[3] = 0;
}

/*
 * Projects the coordinate (op) to infinity in homogenious space from a point (pt)
 */
void ProjectPoint(real *op,real *pt,real t,real *result)
{
  real dd[3];

  ASubtract3D(op,pt,dd);
  ANormalise3D(dd,dd);
  result[0] = op[0] + t*dd[0];
  result[1] = op[1] + t*dd[1];
  result[2] = op[2] + t*dd[2];
  result[3] = 1.0;
}

/*
 * Projects the coordinate (op) to infinity in homogenious space from a point (pt)
 */
void Project2Plane(real *op,real *pt,real *p_n,real *p_a,real *result)
{
  real dir[3],oa[3],t;

  ASubtract3D(op,pt,dir);
  ASubtract3D(p_a,pt,oa);

  t = ADotProduct3D(oa,p_n)/ADotProduct3D(dir,p_n);

  result[0] = pt[0] + t*dir[0];
  result[1] = pt[1] + t*dir[1];
  result[2] = pt[2] + t*dir[2];
  result[3] = 1.0;
}
