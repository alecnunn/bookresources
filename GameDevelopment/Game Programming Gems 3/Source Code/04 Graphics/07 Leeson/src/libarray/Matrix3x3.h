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
 *                         3x3  Matrix Library                              *
 ****************************************************************************
 * File: Matrix3x3.h                                                        *
 * Date: $Date: 2001/11/08 12:02:00 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.5 $	                                            *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 3x3 matrices.                       *
\****************************************************************************/

#include "libarray.h"

#ifndef _MAXTRX_3X3_
#define MATRIX_3X3_
#ifdef __cplusplus
extern "C"
{
#endif
  bool MInvert3x3(const real *p_operand,real *p_result);
  void MAdd3x3(const real *op1,const real *op2,real *result);
  void MSubtract3x3(const real *op1,const real *op2,real *result);
  void MSDivide3x3(real op1,const real *op2,real *result);
  void MSMultiply3x3(real op1,const real *op2,real *result);
  void MVMultiply3x3(const real *op1,const real *op2,real *result);
  void MMMultiply3x3(const real *op1,const real *op2,real *result);
  void MNegate3x3(const real *op,real *result);
  void MTranspose3x3(const real *op,real *result);
  void MPrint3x3(const real *op);
  void MIdentity3x3(real *M);
  void MRotate3x3(real angle,real *p_axis,real *M);
  real MDeterminant3x3(const real *op);
#ifdef __cplusplus
}
#endif
#endif
