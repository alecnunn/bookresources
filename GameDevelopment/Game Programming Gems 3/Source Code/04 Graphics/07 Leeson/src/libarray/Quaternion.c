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
 * File: Quaternion.cc                                                      *
 * Date: $Date: 2001/12/19 12:56:19 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.10 $                                               *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on quaternions.                        *
\****************************************************************************/

#include "Quaternion.h"

/*
 *	Multiply
 */
void QMultiply(const real *op1,const real *op2,real *result)
{
 /*
  *	w = w1w2 - x1x2 - y1y2 - z1z2
  *	x = w1x2 + x1w2 + y1z2 - z1y2
  *     y = w1y2 - x1z2 + y1w2 + z1x2
  *     z = w1z2 + x1y2 - y1x2 + z1w2
  */
 result[3] = op1[3]*op2[3] - op1[0]*op2[0] - op1[1]*op2[1] - op1[2]*op2[2];
 result[0] = op1[3]*op2[0] + op1[0]*op2[3] + op1[1]*op2[2] - op1[2]*op2[1];
 result[1] = op1[3]*op2[1] - op1[0]*op2[2] + op1[1]*op2[3] + op1[2]*op2[0];
 result[2] = op1[3]*op2[2] + op1[0]*op2[1] - op1[1]*op2[0] + op1[2]*op2[3];
}

/*
 *	Multiply quaternion by scalar
 */
void QSMultiply(const real op1,const real *op2,real *result)
{
 result[0] = op1*op2[0];
 result[1] = op1*op2[1];
 result[2] = op1*op2[2];
 result[3] = op1*op2[3];
}

/*
 *	Add 2 quaternions
 */
void QAdd(const real *op1,const real *op2,real *result)
{
 result[0] = op1[0] + op2[0];
 result[1] = op1[1] + op2[1];
 result[2] = op1[2] + op2[2];
 result[3] = op1[3] + op2[3];
}

/*
 *	Subtract 2 quaternions
 */
void QSub(const real *op1,const real *op2,real *result)
{
 result[0] = op1[0] - op2[0];
 result[1] = op1[1] - op2[1];
 result[2] = op1[2] - op2[2];
 result[3] = op1[3] - op2[3];
}

/*
 *	Conjugate
 */
void QConjugate(const real *op,real *result)
{
 result[0] = -op[0];
 result[1] = -op[1];
 result[2] = -op[2];
 result[3] = op[3];
}

/*
 *	Identity quaternion
 */
void QIdentity(real *result)
{
 result[0] = 0;
 result[1] = 0;
 result[2] = 0;
 result[3] = 1;
}

/*
 *	Print
 */
void QPrint(const real *V)
{
 fprintf(stderr,"(%f [%f %f %f])",V[3],V[0],V[1],V[2]); 
}

/*
 *	Copy
 */
void QCopy(const real *op,real *result)
{
 result[0] = op[0];
 result[1] = op[1];
 result[2] = op[2];
 result[3] = op[3];
}

/*
 *	Norm of a Quaternion
 */
real QNorm(const real *op)
{
 return sqrt(op[0]*op[0] + op[1]*op[1] + op[2]*op[2] + op[3]*op[3]);
}

/*
 *	QInverse
 */
void QInverse(const real *op,real *result)
{
 real n;

 n = 1.0/(op[0]*op[0] + op[1]*op[1] + op[2]*op[2] + op[3]*op[3]);
 result[0] = -n*op[0]; 
 result[1] = -n*op[1]; 
 result[2] = -n*op[2]; 
 result[3] = n*op[3]; 
}

/*
 *	DotProduct
 */
real QDotProduct(const real *op1,const real *op2)
{
 return op1[0]*op2[0] + op1[1]*op2[1] + op1[2]*op2[2] + op1[3]*op2[3];
}

/*
 *         q^t QPow	
 */
void QPow(const real *op,const real t,real *result)
{
 /*
 real theta;

 if(fabs(op[3]) > 1.0)
   printf("qpow error:%f\n",op[3]);
 theta = acos(op[3]);
 VNormalise3DC(op,result);
 ASMultiply3D(sin(t*theta),result,result);
 result[3] = cos(t*theta);
 */
  real a[4],b[4];

  QLn(op,a);
  QSMultiply(t,a,b);
  QExp(b,result);
}

/*
 *	log(q) QLog
 */
void QLog(const real *op,real *result)
{
  QLn(op,result);
  QSMultiply(1.0/log(10),result,result);
  /*
 real theta;

 theta = acos(op[3]);
 VNormalise3DC(op,result);
 ASMultiply3D(theta,result,result);
 result[3] = 0.0; 
  */
}

/*
 * QLn natural log
 */
void QLn(const real *op,real *result)
{
  real abs_v;

  abs_v = QNorm(op);
  if(abs_v == 0)
    {
      if(op[3] > 0)
	{
	  result[0] = 0;
	  result[1] = 0;
	  result[2] = 0;
	  result[3] = log(op[3]);
	}
      else
	{
	  result[0] = M_PI;
	  result[1] = 0;
	  result[2] = 0;
	  result[3] = log(-1*op[3]);
	}
    }
  else
    {
      real c;

      c = atan2(abs_v,op[3])/abs_v;
      result[0] = c*op[0];
      result[1] = c*op[1];
      result[2] = c*op[2];
      result[3] = 0.5*log(op[3]*op[3] + abs_v*abs_v);
    }
}

/*
 *	exp(q) QExp
 */
void QExp(const real *op,real *result)
{
  /*
 real angle,sa,c;

 angle = QNorm(op);
 result[3] = cos(angle);
 sa = sin(angle);
 if(fabs(sa) >= 0.0000001)
   {
    c = sa/angle;
    ASMultiply3D(c,op,result);
   }
 else
   {
    QCopy(op,result);
   }
  */
  real abs_v;

  abs_v = QNorm(op);
  if(abs_v == 0) /* is this a pure scalar */
    {
      result[0] = 0;
      result[1] = 0;
      result[2] = 0;
      result[3] = exp(op[3]);
    }
  else
    {
      real c;

      c = (exp(op[3])*sin(abs_v))/abs_v;
      result[0] = c*op[0];
      result[1] = c*op[1];
      result[2] = c*op[2];
      result[3] = exp(op[3])*cos(abs_v);
    }
}

/*
 *      Quaternion to Matrix
 */
void Quat2M4x4(const real *Q,real *M)
{
 //real wx,wy,wz,xx,yy,zz,yz,xy,xz,x2,y2,z2;

 //printf("QNorm:%f\n",QNorm(Q));
 // determine coefficients
 M[0] = 1 - 2*Q[1]*Q[1] - 2*Q[2]*Q[2]; M[4] = 2*Q[0]*Q[1] - 2*Q[3]*Q[2];     M[ 8] = 2*Q[0]*Q[2] + 2*Q[3]*Q[1];     M[12] = 0.0;
 M[1] = 2*Q[0]*Q[1] + 2*Q[3]*Q[2];     M[5] = 1 - 2*Q[0]*Q[0] - 2*Q[2]*Q[2]; M[ 9] = 2*Q[1]*Q[2] - 2*Q[3]*Q[0];     M[13] = 0.0;
 M[2] = 2*Q[0]*Q[2] - 2*Q[3]*Q[1];     M[6] = 2*Q[1]*Q[2] + 2*Q[3]*Q[0];     M[10] = 1 - 2*Q[0]*Q[0] - 2*Q[1]*Q[1]; M[14] = 0.0;
 M[3] = 0.0;                           M[7] = 0.0;                           M[11] = 0.0;                           M[15] = 1.0;
/*
 x2 = Q[0] + Q[0];
 y2 = Q[1] + Q[1];
 z2 = Q[2] + Q[2];

 xx = Q[0]*x2;
 xy = Q[0]*y2;
 xz = Q[0]*z2;

 yy = Q[1]*y2;
 yz = Q[1]*z2;
 zz = Q[1]*z2;

 wx = Q[3]*x2;
 wy = Q[3]*y2;
 wz = Q[3]*z2;

 M[0] = 1.0 - (yy + zz);
        M[1] = (xy - wz);
 //M[1] = (xy + wz);
        M[2] = (xz + wy);
 //M[2] = (xz - wy);
 M[3] = 0.0;

        M[4] = (xy + wz);
 //M[4] = (xy - wz);
 M[5] = 1.0 - (xx + zz);
        M[6] = (yz - wx);
 //M[6] = (yz + wx);
 M[7] = 0.0;

        M[8] = (xz - wy);
 //M[8] = (xz + wy);
        M[9] = (yz + wx);
 //M[9] = (yz - wx);
 M[10] = 1.0 - (xx + yy);
 M[11] = 0.0;

 M[12] = 0.0;
 M[13] = 0.0;
 M[14] = 0.0;
 M[15] = 1.0;
 */
 /*
 real xx,xy,xz,xw,yy,yz,yw,zz,zw;

 xx = Q[0]*Q[0];
 xy = Q[0]*Q[1];
 xz = Q[0]*Q[2];
 xw = Q[0]*Q[3];

 yy = Q[1]*Q[1];
 yz = Q[1]*Q[2];
 yw = Q[1]*Q[3];

 zz = Q[2]*Q[2];
 zw = Q[2]*Q[3];

 M[0] = 1 - 2*xx + 2*yy;
 M[1] = 2*xy + 2*zw;
 M[2] = 2*xz + 2*yw;
 M[3] = 0.0;

 M[4] = 2*xy - 2*zw;
 M[5] = 1 - 2*xx + 2*zz;
 M[6] = 2*yz + 2*xw;
 M[7] = 0.0;

 M[8] = 2*xz + 2*yw;
 M[9] = 2*yz - 2*xw;
 M[10]= 1 - 2*xx + 2*yy;
 M[11]= 0.0;

 M[12]= 0.0;
 M[13]= 0.0;
 M[14]= 0.0;
 M[15]= 1.0;
 */
}


/*
 *      Quaternion to Matrix
 */
void Quat2M3x3(const real *Q,real *M)
{
 // determine coefficients
  M[0] = 1 - 2*Q[1]*Q[1] - 2*Q[2]*Q[2]; M[3] = 2*Q[0]*Q[1] - 2*Q[3]*Q[2];     M[6] = 2*Q[0]*Q[2] + 2*Q[3]*Q[1];
  M[1] = 2*Q[0]*Q[1] + 2*Q[3]*Q[2];     M[4] = 1 - 2*Q[0]*Q[0] - 2*Q[2]*Q[2]; M[7] = 2*Q[1]*Q[2] - 2*Q[3]*Q[0];
  M[2] = 2*Q[0]*Q[2] - 2*Q[3]*Q[1];     M[5] = 2*Q[1]*Q[2] + 2*Q[3]*Q[0];     M[8] = 1 - 2*Q[0]*Q[0] - 2*Q[1]*Q[1];
}

/*
 * Quaternion to Euler conversion
 */
void Quat2Euler(const real *op,real *result)
{
  real M[16];

  Quat2M4x4(op,M);
  MMat2Euler4x4(M,result);
}

/*
 *      Axis angle to quaternion
 */
void AA2Quat(const real *axis,real angle,real *result)
{
 angle /= 2.0;
 ASMultiply3D(sin(angle),axis,result);
 result[3] = cos(angle);
}

/*
 *		Quaternion to axis angle
 */
void Quat2AA(const real *Q,real *p_angle,real *p_axis)
{
  real m;
  /*
   *p_angle = 2.0*acos(Q[3]);
   m = VMagnitude3DC(Q);
   if(m != 0.0)
   {
   ASDivide3D(m,Q,p_axis);
   }
   else
   {
   // any angle will do cos there is no rotation
   p_axis[0] = 0;
   p_axis[1] = 1;
   p_axis[2] = 0;
   }
  */
  if(fabs(Q[3]) < 1.0)
    {
      *p_angle = 2.0*acos(Q[3]);
      m = sqrt(1.0 - Q[3]*Q[3]);
      ASDivide3D(m,Q,p_axis);
    }
  else
    {
      // any angle will do cos there is no rotation
      p_axis[0] = 0;
      p_axis[1] = 1;
      p_axis[2] = 0;
      *p_angle = 0.0;
    }
}

/*
 *      Calculate Difference Between Transforms
 */
 void QDifference(real *Q1,real *Q2,real *D)
 {
  real C[4];

  QInverse(Q1,C);
  QMultiply(C,Q2,D);
 }

/*
 *      SLERP - use some hacks so do not use as a real SLERP function
 */
void slerp(const real *q0,const real *q1,const real t,real *result)
{
  /*
 real qa[4],qb[4];//,n;

 QInverse(q0,qa);
 //QPow(q0,-1,qa);
 QMultiply(qa,q1,qb);
 QPow(qb,t,qa);
 QMultiply(q0,qa,result);
  */
 /*
 n = QNorm(qa);
 if(fabs(n) - 1.0 > 0.00001)
   {
    printf("arghh %f\n",n);
   }
 */
 real angle,ca,sa,Q1[4],Q2[4],s1,s2;

 //QPrint(q1); printf(" "); QPrint(q2); printf("\n");
 ca = QDotProduct(q0,q1);
 if(ca < 0.0)
   {
    ca = -ca;
    Q1[0] = -q0[0];
    Q1[1] = -q0[1];
    Q1[2] = -q0[2];
    Q1[3] = -q0[3];
   }
 else
   {
    QCopy(q0,Q1);
   }
 if((1.0 - ca) > 0.000000001)
   {
    angle = acos(ca);
    sa = sin(angle);
    //if(sa > 0.0)
      {
       s1 = sin((1.0 - t)*angle)/sa;
       s2 = sin(t*angle)/sa;
      }
   }
 else
   {
    s1 = 1.0 - t;
    s2 = t;
   }
 QSMultiply(s1,Q1,Q1);
 QSMultiply(s2,q1,Q2);
 QAdd(Q1,Q2,result);
/*
 real n = QNorm(result);
 if(fabs(1.0 - n) > 0.000001)
   {
    printf("Norm:%f\n",n);
   }
 */
}

/*
 *      SLERP without shortest angle hack and linear interpolation for very small steps
 */
void rslerp(const real *q0,const real *q1,const real t,real *result)
{
  /*
 real qa[4],qb[4];//,n;

 QInverse(q0,qa);
 //QPow(q0,-1,qa);
 QMultiply(qa,q1,qb);
 QPow(qb,t,qa);
 QMultiply(q0,qa,result);
  */
 /*
 n = QNorm(qa);
 if(fabs(n) - 1.0 > 0.00001)
   {
    printf("arghh %f\n",n);
   }
 */
 real angle,ca,sa,Q1[4],Q2[4],s1,s2;

 ca = QDotProduct(q0,q1);
 if((1.0 - ca) > 0.000000001)
   {
     angle = acos(ca);
     sa = sin(angle);
     s1 = sin((1.0 - t)*angle)/sa;
     s2 = sin(t*angle)/sa;
     
   }
else
  {
    s1 = 1 - t;
    s2 = t;
  }
 QSMultiply(s1,q0,Q1);
 QSMultiply(s2,q1,Q2);
 QAdd(Q1,Q2,result);

/*
 real n = QNorm(result);
 if(fabs(1.0 - n) > 0.000001)
   {
    printf("Norm:%f\n",n);
   }
 */
}

/*
 *      SQUAD
 */
void squad(const real *p,const real *q,const real *a,const real *b,const real t,real *result)
{
 real Q1[4],Q2[4];

 //printf("\t\tp:"); QPrint(p); printf("\n");
 //printf("\t\tq:"); QPrint(q); printf("\n");
 rslerp(p,q,t,Q1);
 //printf("\tQ1(%f):",u); QPrint(Q1); printf("\n");

 //printf("\t\ta:"); QPrint(a); printf("\n");
 //printf("\t\tb:"); QPrint(b); printf("\n");
 rslerp(a,b,t,Q2);
 //printf("\tQ2(%f):",u); QPrint(Q2); printf("\n");
 /*
 if(Q1[3] > 1.0)
    Q1[3] = 1.0;
 else if(Q1[3] < -1.0)
    Q1[3] = -1.0;

 if(Q2[3] > 1.0)
    Q2[3] = 1.0;
 else if(Q2[3] < -1.0)
    Q2[3] = -1.0;
 */
 //printf("\t\ta:"); QPrint(Q1); printf("\n");
 //printf("\t\tb:"); QPrint(Q2); printf("\n");
 rslerp(Q1,Q2,2*t*(1.0 - t),result);
 //printf("\tresult(%f):",2*u*(1.0 - u)); QPrint(result); printf("\n");
}

/*
 *      Generates C2 continuous control points
 */
void C2Control(const real *op0,const real *op1,const real *op2,real *result)
{
 real qa[4],qb[4],a[4],b[4];

 QInverse(op1,qa);

 QMultiply(qa,op0,qb);
 QLog(qb,a);

 QMultiply(qa,op2,qb);
 QLog(qb,b);

 QSub(b,a,qa);
 QSMultiply(0.25,qa,qa);
 QExp(qa,qb);

 QMultiply(op1,qb,result);
}

/*
 *      SQUAD control point
 */
void QSpline(const real *op0,const real *op1,const real *op2,const real *op3,const real t,real *result)
{
 real a[4],b[4];

 C2Control(op0,op1,op2,a);
 //printf("a:"); QPrint(a); printf("\n");
 C2Control(op1,op2,op3,b);
 //printf("b:"); QPrint(b); printf("\n");
 squad(op1,op2,a,b,t,result);
 //printf("result(%f):",t); QPrint(result); printf("\n");
}

/*
 *  Discrete piecewise function (i.e. no interpoltion)
 */
void Discrete_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q)
{
  int k;

  //printf("%f -> ",t);
  t = t - (p_params[n - 1] - p_params[0])*(floor((t - p_params[0])/(p_params[n - 1] - p_params[0])));
  //t = Periodic(t,p_params[0],p_params[n - 1]);
  //printf("%f\n",t);

  k = FindIndex(t,n,p_params);  
  /* fprintf(stderr,"(%d,%f) ",k,p_params[k]); */
  QCopy(p_nodes + k*4,Q);
  /*printf("%f - [%d] ",t,k);
  QPrint(Q);
  printf("\n");*/
}

/*
 * SLERP piecewise function
 */
void SLERP_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q)
{
  int k_1,k_2;

  t = t - (p_params[n - 1] - p_params[0])*(floor((t - p_params[0])/(p_params[n - 1] - p_params[0])));

  k_1 = FindIndex(t,n,p_params);
  k_2 = (k_1 + 1) % n;
  /* printf("[%d:%f]",k_1,t); */
  if(k_1 < k_2)
    t = (t - p_params[k_1])/(p_params[k_2] - p_params[k_1]);
  else
    t = (t - p_params[k_1])/p_params[k_2];

  /* fprintf(stderr,"(%d,%f) ",k,t); */
  rslerp(p_nodes + k_1*4,p_nodes + k_2*4,t,Q);
  /*
    printf("%f{%f %f} -",t,p_params[k_1],p_params[k_2]);
    QPrint(Q);
    printf("\n");
  */
}

/*
 *  Bezier piecewise function
 */
void Bezier_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q)
{
  int f_0,f_1,f_2,f_3;
  real a[4],b[4];

  t = t - (p_params[n - 1] - p_params[0])*(floor((t - p_params[0])/(p_params[n - 1] - p_params[0])));

  /*
    printf("\t");
    for(i = 0;i < n;i++)
    printf("%d:%1.2f ",i,p_params[i]);
    printf("\n");
  */
  
  f_1 = FindIndex(t,n,p_params);
  
  if(f_1 > 0)
    f_0 = ((f_1 - 1) % n);
  else
    f_0 = n - 1;

  /* f_1 = ((f_2 - 1) % n); */

  f_2 = ((f_1 + 1) % n);
  f_3 = ((f_1 + 2) % n);
  
  if(f_1 < f_2)
    t = (t - p_params[f_1])/(p_params[f_2] - p_params[f_1]);
  else
    t = (t - p_params[f_1])/p_params[f_2];
  
  /* printf("(%f){%d %d %d %d}{%f}\n",t,f_0,f_1,f_2,f_3,p_params[f_2] - p_params[f_1]); */

  C2Control(p_nodes + f_0*4,p_nodes + f_1*4,p_nodes + f_2*4,a);
  C2Control(p_nodes + f_1*4,p_nodes + f_2*4,p_nodes + f_3*4,b);
  squad(p_nodes + f_1*4,p_nodes + f_2*4,a,b,t,Q);
}

/*
 *  CatmullRom piecewise function
 */
void CatmullRom_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q)
{
  int f_0,f_1,f_2,f_3;
  real q10[4],q11[4],q12[4],q20[4],q21[4];

  t = t - (p_params[n - 1] - p_params[0])*(floor((t - p_params[0])/(p_params[n - 1] - p_params[0])));

  /*
    printf("\t");
    for(i = 0;i < n;i++)
    printf("%d:%1.2f ",i,p_params[i]);
    printf("\n");
  */
  
  f_1 = FindIndex(t,n,p_params);
  
  if(f_1 > 0)
    f_0 = ((f_1 - 1) % n);
  else
    f_0 = n - 1;

  /* f_1 = ((f_2 - 1) % n); */

  f_2 = ((f_1 + 1) % n);
  f_3 = ((f_1 + 2) % n);
  
  if(f_1 < f_2)
    t = (t - p_params[f_1])/(p_params[f_2] - p_params[f_1]);
  else
    t = (t - p_params[f_1])/p_params[f_2];
  
  /* printf("(%f){%d %d %d %d}{%f}\n",t,f_0,f_1,f_2,f_3,p_params[f_2] - p_params[f_1]); */
  
  rslerp(p_nodes + f_0*4,p_nodes + f_1*4,t + 1,q10);
  /* QPrint(q10); printf(" %f\n",t + 1); */
  
  rslerp(p_nodes + f_1*4,p_nodes + f_2*4,t,q11);
  /* QPrint(q11); printf(" %f\n",t); */
  
  rslerp(p_nodes + f_2*4,p_nodes + f_3*4,t - 1,q12);
  /* QPrint(q12); printf(" %f\n",t - 1); */

  rslerp(q11,q12,t/2,q21);
  /* QPrint(q21); printf(" %f\n",t/2); */
  
  rslerp(q10,q11,(t + 1)/2,q20);
  /* QPrint(q10);QPrint(q11);QPrint(q20); printf(" %f\n",(t + 1)/2); */

  rslerp(q20,q21,t,Q);  
  /*
    }
    else
    QCopy(p_nodes + f_1*4,Q);    
  */
}

/*
 *  CatmullRom piecewise function
 */
void BSpline_Evaluate(real t,integer n,real *p_params,real *p_nodes,real *Q)
{
  int f_0,f_1,f_2,f_3;
  real q10[4],q11[4],q12[4],q20[4],q21[4];

  t = t - (p_params[n - 1] - p_params[0])*(floor((t - p_params[0])/(p_params[n - 1] - p_params[0])));

  /*
    printf("\t");
    for(i = 0;i < n;i++)
    printf("%d:%1.2f ",i,p_params[i]);
    printf("\n");
  */
  
  f_1 = FindIndex(t,n,p_params);
  
  if(f_1 > 0)
    f_0 = ((f_1 - 1) % n);
  else
    f_0 = n - 1;

  /* f_1 = ((f_2 - 1) % n); */

  f_2 = ((f_1 + 1) % n);
  f_3 = ((f_1 + 2) % n);
  
  if(f_1 < f_2)
    t = (t - p_params[f_1])/(p_params[f_2] - p_params[f_1]);
  else
    t = (t - p_params[f_1])/p_params[f_2];
  
  /* printf("(%f){%d %d %d %d}{%f}\n",t,f_0,f_1,f_2,f_3,p_params[f_2] - p_params[f_1]); */
  
  rslerp(p_nodes + f_0*4,p_nodes + f_1*4,(t + 2.0)/3.0,q10);
  /* QPrint(q10); printf(" %f\n",t + 1); */
  
  rslerp(p_nodes + f_1*4,p_nodes + f_2*4,(t + 1.0)/3.0,q11);
  /* QPrint(q11); printf(" %f\n",t); */
  
  rslerp(p_nodes + f_2*4,p_nodes + f_3*4,t/3.0,q12);
  /* QPrint(q12); printf(" %f\n",t - 1); */

  rslerp(q11,q12,(t + 1.0)/2.0,q21);
  /* QPrint(q21); printf(" %f\n",t/2); */
  
  rslerp(q10,q11,t/2.0,q20);
  /* QPrint(q10);QPrint(q11);QPrint(q20); printf(" %f\n",(t + 1)/2); */

  rslerp(q20,q21,t,Q);  
  /*
    }
    else
    QCopy(p_nodes + f_1*4,Q);    
  */
}

void QDivide(real *Q1,real *Q2,real *result)
{
  real m;
  real tmp[4];

  m = QNorm(Q2);
  m = m*m;

  ANegate4D(Q2,tmp);
  tmp[3] += 2*Q1[3];
  
  QMultiply(Q1,tmp,result);
}

/*
 * transformas a point by a quaternion
 */
void QTransform(real *Q,real *src,real *dst)
{
  real I[4];

  // pt = Q*pt*Q^-1
  QConjugate(Q,I);
  QMultiply(Q,src,dst);
  QMultiply(dst,I,dst);
}










