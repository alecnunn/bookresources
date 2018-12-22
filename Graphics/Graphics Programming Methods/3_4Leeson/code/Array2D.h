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
 *                         2 Dimensional Array Function Library             *
 ****************************************************************************
 * File: Array2D.h                                                          *
 * Date: $Date: 2002/08/14 14:30:05 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.1.1.1 $                                                *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 2 dim arrays.                       *
\****************************************************************************/
#if HAVE_CONFIG_H
   #include <config.h>
#endif

#include "libarray.h"

#ifndef _ARRAY_2D_
#define _ARRAY_2D_

#ifdef __cplusplus
extern "C"
{
#endif
  // Standard vector stuff
  void AZero2D(real *op);
  void AAdd2D(const real *op,const real *op2,real *result);
  void ASubtract2D(const real *op1,const real *op2,real *result);
  void ANegate2D(const real *op,real *result);
  void ASMultiply2D(real op1,const real *op2,real *result);
  void AAMultiply2D(const real *op1,const real *op2,real *result);
  void AMMultiply2D(const real *op1,const real *op2,real *result);
  void ASDivide2D(real op1,const real *op2,real *result);
  void AVDivide2D(real *op1,const real *op2,real *result);
  void ACopy2D(const real *p_scr,real *p_dest);
  void AAbs2D(const real *p_op,real *p_result);
  bool ACompare2D(const real *p_op1,const real *p_op2);
  void APrint2D(const real *op);
  void AMinElements2D(const real *op1,const real *op2,real *result);
  void AMaxElements2D(const real *op1,const real *op2,real *result);
  real AMagnitude2D(const real *op);
  real ADotProduct2D(const real *op1,const real *op2);
  void ALerp2D(real *op1,real *op2,real t,real *result);
#ifdef __cplusplus
}
  #endif
#endif


