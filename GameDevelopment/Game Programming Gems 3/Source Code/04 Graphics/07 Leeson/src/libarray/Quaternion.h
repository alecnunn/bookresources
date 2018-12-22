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
 *                         Quaternion Function Library                      *
 ****************************************************************************
 * File: Quaternion.h                                                       *
 * Date: $Date: 2001/12/19 12:56:18 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.6 $                                                *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on quaternions.                        *
\****************************************************************************/
#if HAVE_CONFIG_H
   #include <config.h>
#endif

#include "libarray.h"

#ifndef _QUATERNION_VECTOR_
  #define _QUATERNION_VECTOR_

  #ifdef __cplusplus
  extern "C"
  {
#endif
    void QMultiply(const real *op1,const real *op2,real *result);
    void QSMultiply(const real op1,const real *op2,real *result);
    void QAdd(const real *op,const real *op2,real *result);
    void QSub(const real *op1,const real *op2,real *result);
    void QConjugate(const real *op,real *result);
    //void QNorm(const real *op,real *result);
    void QIdentity(real *result);
    void QPrint(const real *V);
    void QCopy(const real *op,real *result);
    void QInverse(const real *p,real *result);
    real QDotProduct(const real *op1,const real *op2);
    real QNorm(const real *op);
    void QPow(const real *op,const real t,real *result);
    void QLn(const real *op,real *result);
    void QLog(const real *op,real *result);
    void QExp(const real *op,real *result);
    void Quat2Euler(const real *op,real *result);
    void Quat2M4x4(const real *Q,real *M);
    void Quat2M3x3(const real *Q,real *M);
    void AA2Quat(const real *axis,real angle,real *result);
    void Quat2AA(const real *Q,real *p_angle,real *p_axis);
    void QDifference(real *Q1,real *Q2,real *D);
    void QDivide(real *Q1,real *Q2,real *result);
    void QTransform(real *Q,real *src,real *dst);
    
    // interpolation stuff
    void slerp(const real *q1,const real *q2,const real t,real *result);
    void rslerp(const real *q1,const real *q2,const real t,real *result);
    void QSpline(const real *op0,const real *op1,const real *op2,const real *op3,const real t,real *result);
    void squad(const real *p,const real *q,const real *a,const real *b,const real t,real *result);
    void C2Control(const real *op0,const real *op1,const real *op2,real *result);

    // Piecewise functions
    void SLERP_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q);
    void Discrete_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q);
    void Bezier_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q);
    void CatmullRom_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q);
    void BSpline_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q);
#ifdef __cplusplus
  }
  #endif
#endif
