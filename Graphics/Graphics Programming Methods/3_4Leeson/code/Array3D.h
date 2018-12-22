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
 *                         3 Dimensional Array Function Library             *
 ****************************************************************************
 * File: Array3D.h                                                          *
 * Date: $Date: 2002/08/14 14:30:05 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.1.1.1 $                                            *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 3 dim arrays.                       *
\****************************************************************************/
#if HAVE_CONFIG_H
#include <config.h>
#endif

#include "libarray.h"

#ifndef _ARRAY_3D_
#define _ARRAY_3D_
#ifdef __cplusplus
extern "C"
{
#endif
  // Standard vector stuff
  void AZero3D(real *op);
  void AAdd3D(const real *op,const real *op2,real *result);
  void ASubtract3D(const real *op1,const real *op2,real *result);
  void ANegate3D(const real *op,real *result);
  void ASMultiply3D(real op1,const real *op2,real *result);
  void AAMultiply3D(const real *op1,const real *op2,real *result);
  void AMMultiply3D(const real *op1,const real *op2,real *result);
  void ASDivide3D(real op1,const real *op2,real *result);
  void AVDivide3D(real *op1,const real *op2,real *result);
  void ACopy3D(const real *p_scr,real *p_dest);
  void AAbs3D(const real *p_op,real *p_result);
  bool ACompare3D(const real *p_op1,const real *p_op2);
  void APrint3D(const real *op);
  void AMinElements3D(const real *op1,const real *op2,real *result);
  void AMaxElements3D(const real *op1,const real *op2,real *result);
  real ADotProduct3D(const real *op1,const real *op2);
  void ACrossProduct3D(const real *op1,const real *op2,real *result);
  real AMagnitude3D(const real *op);
  void ANormalise3D(const real *op,real *result);
  void ALerp3D(real *op1,real *op2,real t,real *result);
  void AHermite3D(real *p1,real *m1,real *p2,real *m2,real t,real *result);
#ifdef __cplusplus
}
#endif
#endif


