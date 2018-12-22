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
 *                         4x4  Matrix Library                              *
 ****************************************************************************
 * File: Matrix4x4.h                                                        *
 * Date: $Date: 2001/11/08 12:03:37 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.8 $	                                            *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 4x4 matrices.                       *
\****************************************************************************/
#if HAVE_CONFIG_H
   #include <config.h>
#endif

#include "libarray.h"

#ifndef _MAXTRX_4X4_
   #define MATRIX_4X4_
   #ifdef __cplusplus
   extern "C"
   {
   #endif
   bool MInvert4x4(const real *p_operand,real *p_result);
   void MAdd4x4(const real *op1,const real *op2,real *result);
   void MSubtract4x4(const real *op1,const real *op2,real *result);
   void MSDivide4x4(real op1,const real *op2,real *result);
   void MSMultiply4x4(real op1,const real *op2,real *result);
   void MVMultiply4x4(const real *op1,const real *op2,real *result);
   void MPMultiply4x4(const real *op1,const real *op2,real *result);
   void MMMultiply4x4(const real *op1,const real *op2,real *result);
   void MNegate4x4(const real *op,real *result);
   void MTranspose4x4(const real *op,real *result);
   void MPrint4x4(const real *op);
   void MTranslate4x4(real x,real y, real z,real *result);
   void MScale4x4(real x,real y, real z,real *result);
   void MOrtho4x4(real l,real r,real b,real t,real n,real f,real *M);
   void MFrustrum4x4(real l,real r,real b,real t,real n,real f,real *M);
   void MPerspective4x4(real fovy,real aspect,real znear,real zfar,real *M);
   void MLookAt4x4(real *eye,real *center,real *up,real *M);
   void MIdentity4x4(real *M);
   void MCopy4x4(real *src,real *dest);
   void MMat2Euler4x4(const real *M,real *result);
   void MSwopXZ4x4(real *M);
   void MSwopZY4x4(real *M);
   void MRotate4x4(real angle,real *p_axis,real *M);
   void MTranslatev4x4(real *pos,real *M);
   void MGetOrient4x4(real *M,real *result);
   real MDeterminant4x4(real *M);
   void MCoFactor4x4(const real *M,real *result);
   #ifdef __cplusplus
   }
   #endif
#endif
