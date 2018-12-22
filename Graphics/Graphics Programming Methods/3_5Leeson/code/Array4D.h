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
 *                         4 Dimensional Array Function Library             *
 ****************************************************************************
 * File: Array4D.h
 * Date: $Date: 2002/09/14 17:53:58 $
 * Author: $Author: leesonw $
 * Version: $Revision: 1.2 $
 * Description: This provides a set of functions to handle various
 *              operations performed on 4 dim arrays.
 *
 * Log: $Log: Array4D.h,v $
 * Log: Revision 1.2  2002/09/14 17:53:58  leesonw
 * Log: Added 3 vertex projection functions Project2Infinity, ProjectPoint and Project2Plane
 * Log:
 */
#if HAVE_CONFIG_H
   #include <config.h>
#endif

#include "libarray.h"

#ifndef _ARRAY_4D_
#define _ARRAY_4D_
#ifdef __cplusplus
extern "C"
{
#endif
  // Standard vector stuff
  void AZero4D(real *op);
  void AAdd4D(const real *op,const real *op2,real *result);
  void ASubtract4D(const real *op1,const real *op2,real *result);
  void ANegate4D(const real *op,real *result);
  void ASMultiply4D(real op1,const real *op2,real *result);
  void AAMultiply4D(real op1,const real *op2,real *result);
  void AMMultiply4D(const real *op1,const real *op2,real *result);
  void ASDivide4D(real op1,const real *op2,real *result);
  void AVDivide4D(real *op1,const real *op2,real *result);
  void ACopy4D(const real *p_src,real *p_dest);
  void AAbs4D(const real *p_op,real *p_result);
  bool ACompare4D(const real *p_op1,const real *p_op2);
  void APrint4D(const real *op);
  void AMinElements4D(const real *op1,const real *op2,real *result);
  void AMaxElements4D(const real *op1,const real *op2,real *result);
  real AMagnitude4D(const real *op);
  real ADotProduct4D(const real *op1,const real *op2);
  void ACrossProduct4D(const real *op1,const real *op2,const real *op3,real *result);
  void ANormalise4D(const real *op,real *result);
  void AHomogenise4D(const real *op,real *result);
  void ALerp4D(real *op1,real *op2,real t,real *result);
  void Project2Infinity(real *op,real *pt,real *result);
  void ProjectPoint(real *op,real *pt,real t,real *result);
  void Project2Plane(real *op,real *pt,real *p_n,real *p_a,real *result);
#ifdef __cplusplus
}
  #endif
#endif

