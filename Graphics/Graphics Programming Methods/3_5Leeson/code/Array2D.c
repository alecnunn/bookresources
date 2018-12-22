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
 *                            2D Array Function Library                     *
 ****************************************************************************
 * File: Array2D.c                                                          *
 * Date: $Date: 2002/08/14 14:30:05 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.1.1.1 $                                                *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 2D array.                           *
\****************************************************************************/
#include "Array2D.h"

/*
 * Zero a vector
 */
void AZero2D(real *op)
{
  op[0] = 0.0;
  op[1] = 0.0;
}

/*
 * Copy
 */
void ACopy2D(const real *src,real *dest)
{
  dest[0] = src[0];
  dest[1] = src[1];
}

/*
 * Compare
 */
bool ACompare2D(const real *op1,const real *op2)
{

  return ((fabs(op1[0] - op2[0]) < EPSILON) && (fabs(op1[1] - op2[1]) < EPSILON));
}

/*
 * Add
 */
void AAdd2D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] + op2[0];
  result[1] = op1[1] + op2[1];
}

/*
 * Subtract
 */
void ASubtract2D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] - op2[0];
  result[1] = op1[1] - op2[1];
}

/*
 * Array Multiplication by a Scalar
 */
void ASMultiply2D(real op1,const real *op2,real *result)
{
  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
}

/*
 * Array Multiplication by an Array
 */
void AAMultiply2D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0];
  result[1] = op1[1]*op2[1];
}

/*
 * Array Division by a Scalar
 */
void ASDivide2D(real op1,const real *op2,real *result)
{
  op1 = 1.0/op1;
  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
}

/*
 * Array Negation
 */
void ANegate2D(const real *op,real *result)
{
  result[0] = -op[0];
  result[1] = -op[1];
}

/*
 * Array by a Matrix
 */
void AMMultiply2D(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[1]*op2[3] + op1[2]*op2[6];
  result[1] = op1[0]*op2[1] + op1[1]*op2[4] + op1[2]*op2[7];
}

/*
 *      Print 2D vector
 */
void APrint2D(const real *op)
{
  fprintf(stderr,"[%f %f]",op[0],op[1]);
}

/*
 *Determine min elements
 */
void AMinElements2D(const real *op1,const real *op2,real *result)
{
  result[0] = (op1[0] < op2[0]) ? op1[0] : op2[0];
  result[1] = (op1[1] < op2[1]) ? op1[1] : op2[1];
}

/*
 *Determine max elements
 */
void AMaxElements2D(const real *op1,const real *op2,real *result)
{
  result[0] = (op1[0] > op2[0]) ? op1[0] : op2[0];
  result[1] = (op1[1] > op2[1]) ? op1[1] : op2[1];
}

/*
 * Absolute
 */
void AAbs2D(const real *op,real *result)
{
  result[0] = fabs(op[0]);
  result[1] = fabs(op[1]);
}

/*
 * Magnitude of a 2D vector
 */
real AMagnitude2D(const real *op)
{
  return sqrt(op[0]*op[0] + op[1]*op[1]);
}

/*
 * DotProduct of a 2D vector
 */
real ADotProduct2D(const real *op1,const real *op2)
{
  return op1[0]*op2[0] + op1[1]*op2[1];
}

/*
 * Make a unit vector of a 2D array
 */
void ANormalise2D(const real *op,real *result)
{
  real m;
  
  m = 1.0/sqrt(op[0]*op[0] + op[1]*op[1]);
  result[0] = m*op[0];
  result[1] = m*op[1];
}

/*
 * lerp - linear interpolation between 2 vectors
 */
void Alerp2D(real *op1,real *op2,real t,real *result)
{
  result[0] = t*op1[0] + (1 - t)*op2[0];
  result[1] = t*op1[1] + (1 - t)*op2[1];
}

