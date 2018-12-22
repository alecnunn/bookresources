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
 *                            3D Array Function Library                     *
 ****************************************************************************
 * File: Array3D.h                                                          *
 * Date: $Date: 2002/08/14 14:30:05 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.1.1.1 $                                                *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 3D array.                           *
\****************************************************************************/
#include "Array3D.h"

/*
 * Zero a vector
 */
void AZero3D(real *op)
{
  op[0] = 0.0;
  op[1] = 0.0;
  op[2] = 0.0;
}

/*
 * Copy
 */
void ACopy3D(const real *src,real *dest)
{
  dest[0] = src[0];
  dest[1] = src[1];
  dest[2] = src[2];
}

/*
 * Compare
 */
bool ACompare3D(const real *op1,const real *op2)
{
  return ((fabs(op1[0] - op2[0]) < EPSILON) && 
	  (fabs(op1[1] - op2[1]) < EPSILON) && 
	  (fabs(op1[2] - op2[2]) < EPSILON));
}

/*
 * Add
 */
void AAdd3D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] + op2[0];
  result[1] = op1[1] + op2[1];
  result[2] = op1[2] + op2[2];
}

/*
 * Subtract
 */
void ASubtract3D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] - op2[0];
  result[1] = op1[1] - op2[1];
  result[2] = op1[2] - op2[2];
}

/*
 * Array Dividion by an array
 */
void AVDivide3D(real *op1,const real *op2,real *result)
{
  result[0] = op1[0]/op2[0];
  result[1] = op1[1]/op2[1];
  result[2] = op1[2]/op2[2];
}

/*
 * Array Multiplication by a Scalar
 */
void ASMultiply3D(real op1,const real *op2,real *result)
{
  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
  result[2] = op1*op2[2];
}

/*
 * Array Multiplication by an Array
 */
void AAMultiply3D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0];
  result[1] = op1[1]*op2[1];
  result[2] = op1[2]*op2[2];
}

/*
 * Array Division by a Scalar
 */
void ASDivide3D(real op1,const real *op2,real *result)
{
  op1 = 1.0/op1;
  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
  result[2] = op1*op2[2];
}

/*
 * Array Negation
 */
void ANegate3D(const real *op,real *result)
{
  result[0] = -op[0];
  result[1] = -op[1];
  result[2] = -op[2];
}

/*
 * Array by a Matrix
 */
void AMMultiply3D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[1]*op2[3] + op1[2]*op2[6];
  result[1] = op1[0]*op2[1] + op1[1]*op2[4] + op1[2]*op2[7];
  result[2] = op1[0]*op2[2] + op1[1]*op2[5] + op1[2]*op2[8];
}

/*
 *      Print 3D vector
 */
void APrint3D(const real *op)
{
  fprintf(stderr,"[%f %f %f]",op[0],op[1],op[2]);
}

/*
 *Determine min elements
 */
void AMinElements3D(const real *op1,const real *op2,real *result)
{
  result[0] = (op1[0] < op2[0]) ? op1[0] : op2[0];
  result[1] = (op1[1] < op2[1]) ? op1[1] : op2[1];
  result[2] = (op1[2] < op2[2]) ? op1[2] : op2[2];
}

/*
 *Determine max elements
 */
void AMaxElements3D(const real *op1,const real *op2,real *result)
{
  result[0] = (op1[0] > op2[0]) ? op1[0] : op2[0];
  result[1] = (op1[1] > op2[1]) ? op1[1] : op2[1];
  result[2] = (op1[2] > op2[2]) ? op1[2] : op2[2];
}

/*
 * Absolute
 */
void AAbs3D(const real *op,real *result)
{
  result[0] = fabs(op[0]);
  result[1] = fabs(op[1]);
  result[2] = fabs(op[2]);
}

/*
 *  DotProduct
 */
real ADotProduct3D(const real *op1,const real *op2)
{
  return op1[0]*op2[0] + op1[1]*op2[1] + op1[2]*op2[2];
}

/*
 *  CrossProduct
 */
void ACrossProduct3D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[1]*op2[2] - op1[2]*op2[1];
  result[1] = op1[2]*op2[0] - op1[0]*op2[2];
  result[2] = op1[0]*op2[1] - op1[1]*op2[0];
}

/*
 * Magnitude of a 3D vector
 */
real AMagnitude3D(const real *op)
{
  return sqrt(op[0]*op[0] + op[1]*op[1] + op[2]*op[2]);
}

/*
 * Normalise
 */
void ANormalise3D(const real *op,real *result)
{
  real m;

  m = AMagnitude3D(op);
  ASDivide3D(m,op,result);
}

/*
 * lerp - linear interpolation between 2 vectors
 */
void Alerp3D(real *op1,real *op2,real t,real *result)
{
  result[0] = t*op1[0] + (1 - t)*op2[0];
  result[1] = t*op1[1] + (1 - t)*op2[1];
  result[2] = t*op1[2] + (1 - t)*op2[2];
}

/*
 * Hermite - hermite interpolation between 2 vectors
 */
void AHermite3D(real *p1,real *m1,real *p2,real *m2,real t,real *result)
{
  real a,b,c,d,tmp[3],ttt,tt;

  tt = t*t;
  ttt = t*t*t;
  a = 2*ttt - 3*tt + 1;
  b = ttt - 2*tt + t;
  c = ttt - tt;
  d = -2*ttt + 3*tt;

  ASMultiply3D(a,p1,result);

  ASMultiply3D(b,m1,tmp);
  AAdd3D(tmp,result,result);

  ASMultiply3D(c,m2,tmp);
  AAdd3D(tmp,result,result);

  ASMultiply3D(d,p2,tmp);
  AAdd3D(tmp,result,result);
}
