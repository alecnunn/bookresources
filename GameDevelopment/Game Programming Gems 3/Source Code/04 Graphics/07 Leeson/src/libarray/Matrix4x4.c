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
 * Date: $Date: 2001/11/08 17:35:28 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.9 $	                                            *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 4x4 matrices.                       *
\****************************************************************************/
#include "Matrix4x4.h"

/*
 *                             Matrix Addition
 */
void MAdd4x4(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] + op2[0];
  result[1] = op1[1] + op2[1];
  result[2] = op1[2] + op2[2];
  result[3] = op1[3] + op2[3];

  result[4] = op1[4] + op2[4];
  result[5] = op1[5] + op2[5];
  result[6] = op1[6] + op2[6];
  result[7] = op1[7] + op2[7];

  result[8] = op1[8] + op2[8];
  result[9] = op1[9] + op2[9];
  result[10] = op1[10] + op2[10];
  result[11] = op1[11] + op2[11];

  result[12] = op1[12] + op2[12];
  result[13] = op1[13] + op2[13];
  result[14] = op1[14] + op2[14];
  result[15] = op1[15] + op2[15];
}

/*
 *                             Matrix Subtraction
 */
void MSubtract4x4(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0] - op2[0];
  result[1] = op1[1] - op2[1];
  result[2] = op1[2] - op2[2];
  result[3] = op1[3] - op2[3];

  result[4] = op1[4] - op2[4];
  result[5] = op1[5] - op2[5];
  result[6] = op1[6] - op2[6];
  result[7] = op1[7] - op2[7];

  result[8] = op1[8] - op2[8];
  result[9] = op1[9] - op2[9];
  result[10] = op1[10] - op2[10];
  result[11] = op1[11] - op2[11];

  result[12] = op1[12] - op2[12];
  result[13] = op1[13] - op2[13];
  result[14] = op1[14] - op2[14];
  result[15] = op1[15] - op2[15];
}

/*
 *                             Matrix Negation
 */
void MNegate4x4(const real *op,real *result)
{
  result[0] = -op[0];
  result[1] = -op[1];
  result[2] = -op[2];
  result[3] = -op[3];

  result[4] = -op[4];
  result[5] = -op[5];
  result[6] = -op[6];
  result[7] = -op[7];

  result[8] = -op[8];
  result[9] = -op[9];
  result[10] =-op[10];
  result[11] = -op[11];

  result[12] = -op[12];
  result[13] = -op[13];
  result[14] = -op[14];
  result[15] = -op[15];
}

/*
 *                             Matrix Multiplication
 */
void MMMultiply4x4(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[4]*op2[1] + op1[8]*op2[2] + op1[12]*op2[3];
  result[1] = op1[1]*op2[0] + op1[5]*op2[1] + op1[9]*op2[2] + op1[13]*op2[3];
  result[2] = op1[2]*op2[0] + op1[6]*op2[1] + op1[10]*op2[2] + op1[14]*op2[3];
  result[3] = op1[3]*op2[0] + op1[7]*op2[1] + op1[11]*op2[2] + op1[15]*op2[3];

  result[4] = op1[0]*op2[4] + op1[4]*op2[5] + op1[8]*op2[6] + op1[12]*op2[7];
  result[5] = op1[1]*op2[4] + op1[5]*op2[5] + op1[9]*op2[6] + op1[13]*op2[7];
  result[6] = op1[2]*op2[4] + op1[6]*op2[5] + op1[10]*op2[6] + op1[14]*op2[7];
  result[7] = op1[3]*op2[4] + op1[7]*op2[5] + op1[11]*op2[6] + op1[15]*op2[7];

  result[8] = op1[0]*op2[8] + op1[4]*op2[9] + op1[8]*op2[10] + op1[12]*op2[11];
  result[9] = op1[1]*op2[8] + op1[5]*op2[9] + op1[9]*op2[10] + op1[13]*op2[11];
  result[10] = op1[2]*op2[8] + op1[6]*op2[9] + op1[10]*op2[10] + op1[14]*op2[11];
  result[11] = op1[3]*op2[8] + op1[7]*op2[9] + op1[11]*op2[10] + op1[15]*op2[11];

  result[12] = op1[0]*op2[12] + op1[4]*op2[13] + op1[8]*op2[14] + op1[12]*op2[15];
  result[13] = op1[1]*op2[12] + op1[5]*op2[13] + op1[9]*op2[14] + op1[13]*op2[15];
  result[14] = op1[2]*op2[12] + op1[6]*op2[13] + op1[10]*op2[14] + op1[14]*op2[15];
  result[15] = op1[3]*op2[12] + op1[7]*op2[13] + op1[11]*op2[14] + op1[15]*op2[15];
}

/*
 *                       Vector Matrix Multiplication
 */
void MVMultiply4x4(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[4]*op2[1] + op1[8]*op2[2]  + op1[12]*op2[3];
  result[1] = op1[1]*op2[0] + op1[5]*op2[1] + op1[9]*op2[2]  + op1[13]*op2[3];
  result[2] = op1[2]*op2[0] + op1[6]*op2[1] + op1[10]*op2[2] + op1[14]*op2[3];
  result[3] = op1[3]*op2[0] + op1[7]*op2[1] + op1[11]*op2[2] + op1[15]*op2[3];
}

/*
 *                       Point Matrix Multiplication
 */
void MPMultiply4x4(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[4]*op2[1] + op1[8]*op2[2];//  + op1[12]*op2[3];
  result[1] = op1[1]*op2[0] + op1[5]*op2[1] + op1[9]*op2[2];//  + op1[13]*op2[3];
  result[2] = op1[2]*op2[0] + op1[6]*op2[1] + op1[10]*op2[2];// + op1[14]*op2[3];
  //result[3] = op1[3]*op2[0] + op1[7]*op2[1] + op1[11]*op2[2] + op1[15]*op2[3];
}

/*
 *   Transpose a matrix
 */
void MTranspose4x4(const real *op,real *result)
{
  result[ 0] = op[ 0];
  result[ 1] = op[ 4];
  result[ 2] = op[ 8];
  result[ 3] = op[12];

  result[ 4] = op[ 1];
  result[ 5] = op[ 5];
  result[ 6] = op[ 9];
  result[ 7] = op[13];

  result[ 8] = op[ 2];
  result[ 9] = op[ 6];
  result[10] = op[10];
  result[11] = op[14];

  result[12] = op[ 3];
  result[13] = op[ 7];
  result[14] = op[11];
  result[15] = op[15];
}

/*
 *		Matrix by a scalar
 */
void MSMultiply4x4(real op1,const real *op2,real *result)
{
  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
  result[2] = op1*op2[2];
  result[3] = op1*op2[3];

  result[4] = op1*op2[4];
  result[5] = op1*op2[5];
  result[6] = op1*op2[6];
  result[7] = op1*op2[7];

  result[8] = op1*op2[8];
  result[9] = op1*op2[9];
  result[10] = op1*op2[10];
  result[11] = op1*op2[11];

  result[12] = op1*op2[12];
  result[13] = op1*op2[13];
  result[14] = op1*op2[14];
  result[15] = op1*op2[15];
}

/*
 *		Matrix divided by a scalar
 */
void MSDivide4x4(real op1,const real *op2,real *result)
{
  op1 = 1.0/op1;

  result[0] = op1*op2[0];
  result[1] = op1*op2[1];
  result[2] = op1*op2[2];
  result[3] = op1*op2[3];

  result[4] = op1*op2[4];
  result[5] = op1*op2[5];
  result[6] = op1*op2[6];
  result[7] = op1*op2[7];

  result[8] = op1*op2[8];
  result[9] = op1*op2[9];
  result[10] = op1*op2[10];
  result[11] = op1*op2[11];

  result[12] = op1*op2[12];
  result[13] = op1*op2[13];
  result[14] = op1*op2[14];
  result[15] = op1*op2[15];
}

/*
 *  Invert a Matrix
 */
bool MInvert4x4(const real *operand,real *result)
{
  integer i,j,k;
  real tmp_mat[4*4];
  real tmp_d;

  memcpy(tmp_mat,operand,sizeof(real)*4*4);

  // Unit Matrix
  MIdentity4x4(result);

  for(i = 0; i < 4; i++)
    {
      for(j = i + 1, k = i; j < 4; j++)
        {
	  if(fabs(tmp_mat[j + i*4]) > fabs(tmp_mat[k + i*4]))
	    {
	      k = j;
	    }
	}
      // check for singularity
      if(tmp_mat[k + i*4] == 0.0)
	{
	  return false;
	}
      // pivot - switch rows k and i
      if(k != i)
	{
	  for(j = 0;j < 4; j++)
            {
	      tmp_d = tmp_mat[i + j*4];
	      tmp_mat[i + j*4] = tmp_mat[k + j*4];
	      tmp_mat[k + j*4] = tmp_d;
	      tmp_d = result[i + j*4];
	      result[i + j*4] = result[k + j*4];
	      result[k + j*4] = tmp_d;
            }
	}
      // normalize the row - make the diagonal 1
      for(tmp_d = 1.0/tmp_mat[i + i*4],j = 0;j < 4;j++)
        {
	  tmp_mat[i + j*4] = tmp_mat[i + j*4]*tmp_d;
	  result[i + j*4] = result[i + j*4]*tmp_d;
        }
      // zero the non-diagonal terms in this column
      for(j = 0;j < 4;j++)
        {
	  if(j != i)
	    {
	      for(tmp_d = -tmp_mat[j + i*4],k = 0;k < 4;k++)
		{
		  tmp_mat[j + k*4] = tmp_mat[j + k*4] + tmp_mat[i + k*4]*tmp_d;
		  result[j + k*4] = result[j + k*4] + result[i + k*4]*tmp_d;
		}
	    }
        }
    }
  return true;
}

/*
 *	Translation Matrix
 */
void MTranslate4x4(real x,real y,real z,real *result)
{
  result[0] = 1.0;
  result[1] = 0.0;
  result[2] = 0.0;
  result[3] = 0.0;

  result[4] = 0.0;
  result[5] = 1.0;
  result[6] = 0.0;
  result[7] = 0.0;

  result[8] = 0.0;
  result[9] = 0.0;
  result[10] = 1.0;
  result[11] = 0.0;

  result[12] = x;
  result[13] = y;
  result[14] = z;
  result[15] = 1.0;
}

/*
 *	Scale Matrix
 */
void MScale4x4(real x,real y,real z,real *result)
{
  result[0] = x;
  result[1] = 0.0;
  result[2] = 0.0;
  result[3] = 0.0;

  result[4] = 0.0;
  result[5] = y;
  result[6] = 0.0;
  result[7] = 0.0;

  result[8] = 0.0;
  result[9] = 0.0;
  result[10] = z;
  result[11] = 0.0;

  result[12] = 0.0;
  result[13] = 0.0;
  result[14] = 0.0;
  result[15] = 1.0;
}

/*
 *MPrint4x4
 */
void MPrint4x4(const real *op)
{
 fprintf(stderr,"|% f % f % f % f|\n",op[0] ,op[4] ,op[8]  ,op[12]);
 fprintf(stderr,"|% f % f % f % f|\n",op[1] ,op[5] ,op[9]  ,op[13]);
 fprintf(stderr,"|% f % f % f % f|\n",op[2] ,op[6] ,op[10] ,op[14]);
 fprintf(stderr,"|% f % f % f % f|\n",op[3] ,op[7] ,op[11] ,op[15]);
}

/*
 *      Orthographic Projection
 */
void MOrtho4x4(real l,real r,real b,real t,real n,real f,real *M)
{
 M[0] = 2.0/(r - l); M[4] = 0.0;         M[ 8] = 0.0;          M[12] = -(r + l)/(r - l);
 M[1] = 0.0;         M[5] = 2.0/(t - b); M[ 9] = 0.0;          M[13] = -(t + b)/(t - b);
 M[2] = 0.0;         M[6] = 0.0;         M[10] = -2.0/(f - n); M[14] = -(f + n)/(f - n);
 M[3] = 0.0;         M[7] = 0.0;         M[11] = 0.0;          M[15] = 1.0;
}

/*
 *      Perspective Projection
 */
void MFrustrum4x4(real l,real r,real b,real t,real n,real f,real *M)
{
 M[0] = (2.0*n)/(r - l); M[4] = 0.0;            M[ 8] = (r + l)/(r - l);  M[12] = 0.0;
 M[1] = 0.0;             M[5] = (2.0*n)/(t -b); M[ 9] = (t + b)/(t - b);  M[13] = 0.0;
 M[2] = 0.0;             M[6] = 0.0;            M[10] = -(f + n)/(f - n); M[14] = -(2.0*f*n)/(f - n);
 M[3] = 0.0;             M[7] = 0.0;            M[11] = -1.0;             M[15] = 0.0;

}

/*
 *      Identity Matrix
 */
void MIdentity4x4(real *M)
{
 M[0] = 1.0; M[4] = 0.0; M[ 8] = 0.0; M[12] = 0.0;
 M[1] = 0.0; M[5] = 1.0; M[ 9] = 0.0; M[13] = 0.0;
 M[2] = 0.0; M[6] = 0.0; M[10] = 1.0; M[14] = 0.0;
 M[3] = 0.0; M[7] = 0.0; M[11] = 0.0; M[15] = 1.0;
}

/*
 *	Copy Matrix
 */
void MCopy4x4(real *src,real *dest)
{
 memcpy(dest,src,sizeof(real)*16);
}

/*
 *      Matrix to Euler
 */
void MMat2Euler4x4(const real *M,real *result)
{
 if((M[4] == 0.0) && (M[4] == M[5]))
   {
    result[0] = 0.0;
    result[2] = atan2(M[4],M[5]);
   }
 else
   {
    result[0] = atan2(-M[2],M[10]);
    result[1] = asin(M[6]);
    result[2] = atan2(-M[4],M[5]);
   }
 /*
 real C,D,x,y;

 result[1] = D = -asin(M[2]);
 C  = cos(result[1]);

 if(fabs(result[1]) > 0.0005)
    {
     x = M[10]/C;
     y = -M[6]/C;

     result[0] = atan2(y,x);

     x = M[0]/C;
     y = -M[1]/C;

     result[2] = atan2(y,x);
    }
  else
    {
    //printf("case 2 (%f)\n",M[2]);
    result[0] = 0;

     x = M[5];
     y = M[4];

     result[2] = atan2(y,x);
    }
 */
}

/*
 *      Orthographic Projection
 */
void MSwopXZ4x4(real *M)
{
 M[0] = 1.0;         M[4] = 0.0;        M[ 8] = 0.0;        M[12] = 0.0;
 M[1] = 0.0;         M[5] = 0.0;        M[ 9] = 1.0;        M[13] = 0.0;
 M[2] = 0.0;         M[6] = 1.0;        M[10] = 0.0;        M[14] = 0.0;
 M[3] = 0.0;         M[7] = 0.0;        M[11] = 0.0;        M[15] = 1.0;
}

/*
 *      Orthographic Projection
 */
void MSwopZY4x4(real *M)
{
 M[0] = 0.0;         M[4] = 0.0;        M[ 8] = 1.0;        M[12] = 0.0;
 M[1] = 0.0;         M[5] = 1.0;        M[ 9] = 0.0;        M[13] = 0.0;
 M[2] = 1.0;         M[6] = 0.0;        M[10] = 0.0;        M[14] = 0.0;
 M[3] = 0.0;         M[7] = 0.0;        M[11] = 0.0;        M[15] = 1.0;

}

/*
 * Rotate around an axis
 */
void MRotate4x4(real angle,real *p_axis,real *M)
{
 real c,s;

 c = cos(angle);
 s = sin(angle);

 M[0] = p_axis[0]*p_axis[0]*(1 - c) + c;           M[4] = p_axis[0]*p_axis[1]*(1 - c) - p_axis[2]*s; M[ 8] = p_axis[0]*p_axis[2]*(1 - c) + p_axis[1]*s;        M[12] = 0.0;
 M[1] = p_axis[1]*p_axis[0]*(1 - c) + p_axis[2]*s; M[5] = p_axis[1]*p_axis[1]*(1 - c) + c;           M[ 9] = p_axis[1]*p_axis[2]*(1 - c) - p_axis[0]*s;        M[13] = 0.0;
 M[2] = p_axis[0]*p_axis[2]*(1 - c) - p_axis[1]*s; M[6] = p_axis[1]*p_axis[2]*(1 - c) + p_axis[0]*s; M[10] = p_axis[2]*p_axis[2]*(1 - c) + c;                  M[14] = 0.0;
 M[3] = 0.0;                                       M[7] = 0.0;                                       M[11] = 0.0;                                              M[15] = 1.0;
}

/*
 * Easier to use perspective projection
 */
void MPerspective4x4(real fovy,real aspect,real znear,real zfar,real *M)
{
 real f;

 f = 1.0/tan(fovy/2.0);

 M[0] = f/aspect;    M[4] = 0.0;        M[ 8] = 0.0;                           M[12] = 0.0;
 M[1] = 0.0;         M[5] = f;          M[ 9] = 0.0;                           M[13] = 0.0;
 M[2] = 0.0;         M[6] = 0.0;        M[10] = (zfar + znear)/(znear - zfar); M[14] = (2*znear*zfar)/(znear - zfar);
 M[3] = 0.0;         M[7] = 0.0;        M[11] = -1.0;                          M[15] = 0.0;
}

/*
 * Look At
 */
void MLookAt4x4(real *eye,real *center,real *up,real *M)
{
  real f[3],UP[3],s[3],u[3],O[16],T[16];

  ASubtract3D(center,eye,f);
  AHomogenise4D(f,f);
  ANormalise3D(f,f);
  AHomogenise4D(up,UP);
  ANormalise3D(UP,UP);
  ACrossProduct3D(f,UP,s);
  ACrossProduct3D(s,f,u);

  O[0] =  s[0];    O[4] =  s[1];       O[ 8] =  s[2];             O[12] = 0.0;
  O[1] =  u[0];    O[5] =  u[1];       O[ 9] =  u[2];             O[13] = 0.0;
  O[2] = -f[0];    O[6] = -f[1];       O[10] = -f[2];             O[14] = 0.0;
  O[3] =   0.0;    O[7] =   0.0;       O[11] =   0.0;             O[15] = 1.0;

  MTranslate4x4(-eye[0],-eye[1],-eye[2],T);
  MMMultiply4x4(T,O,M);
}

/*
 * Translate
 */
void MTranslatev4x4(real *pos,real *M)
{
 M[0] = 1.0; M[4] = 0.0; M[ 8] = 0.0; M[12] = pos[0];
 M[1] = 0.0; M[5] = 1.0; M[ 9] = 0.0; M[13] = pos[1];
 M[2] = 0.0; M[6] = 0.0; M[10] = 1.0; M[14] = pos[2];
 M[3] = 0.0; M[7] = 0.0; M[11] = 0.0; M[15] = 1.0;
}

/*
 * Get the 3x3 orientation matrix from a 4x4 matrix
 */
void MGetOrient4x4(real *M,real *result)
{
  result[0] = M[0]; result[3] = M[4]; result[6] = M[ 8];
  result[1] = M[1]; result[4] = M[5]; result[7] = M[ 9];
  result[2] = M[2]; result[5] = M[6]; result[8] = M[10];
}

/*
 * Determinant
 */
real MDeterminant4x4(real *M)
{
  return M[ 0]*( M[ 5]*(M[10]*M[15] - M[11]*M[14]) 
	       - M[ 9]*(M[ 6]*M[15] - M[ 7]*M[14]) 
	       + M[13]*(M[ 6]*M[11] - M[ 7]*M[10])) 
       - M[ 4]*( M[ 1]*(M[10]*M[15] - M[11]*M[14])
	       - M[ 9]*(M[ 2]*M[15] - M[ 3]*M[14])
	       + M[13]*(M[ 2]*M[11] - M[ 3]*M[10])) 
       + M[ 8]*( M[ 1]*(M[ 6]*M[15] - M[ 7]*M[14])
	       - M[ 5]*(M[ 2]*M[15] - M[ 3]*M[14])
	       + M[13]*(M[ 2]*M[ 7] - M[ 3]*M[ 6])) 
       - M[12]*( M[ 1]*(M[ 6]*M[11] - M[ 7]*M[10])
	       - M[ 5]*(M[ 2]*M[11] - M[ 3]*M[10])
	       + M[ 9]*(M[ 2]*M[ 7] - M[ 3]*M[ 6]));
}

/*
 * CoFactor Matrix
 */
void MCoFactor4x4(const real *M,real *result)
{
  result[ 0] =( M[ 5]*(M[10]*M[15] - M[11]*M[14]) 
	      - M[ 9]*(M[ 6]*M[15] - M[ 7]*M[14]) 
              + M[13]*(M[ 6]*M[11] - M[ 7]*M[10]));
  result[ 4] = -(  M[ 1]*(M[10]*M[15] - M[11]*M[14])
	         - M[ 9]*(M[ 2]*M[15] - M[ 3]*M[14])
	         + M[13]*(M[ 2]*M[11] - M[ 3]*M[10]));
  result[ 8] =   M[ 1]*(M[ 6]*M[15] - M[ 7]*M[14])
               - M[ 5]*(M[ 2]*M[15] - M[ 3]*M[14])
               + M[13]*(M[ 2]*M[ 7] - M[ 3]*M[ 6]);
  result[12] = -(+ M[ 1]*(M[ 6]*M[11] - M[ 7]*M[10])
	         - M[ 5]*(M[ 2]*M[11] - M[ 3]*M[10])
	         + M[ 9]*(M[ 2]*M[ 7] - M[ 3]*M[ 6]));




  result[ 1] = -(  M[ 4]*(M[10]*M[15] - M[11]*M[14]) 
                 - M[ 8]*(M[ 6]*M[15] - M[ 7]*M[14]) 
                 + M[12]*(M[ 6]*M[11] - M[ 7]*M[10]));
  result[ 5] =   M[ 0]*(M[10]*M[15] - M[11]*M[14])
	       - M[ 8]*(M[ 2]*M[15] - M[ 3]*M[14])
	       + M[12]*(M[ 2]*M[11] - M[ 3]*M[10]);
  result[ 9] = -(  M[ 0]*(M[ 6]*M[15] - M[ 7]*M[14])
                 - M[ 4]*(M[ 2]*M[15] - M[ 3]*M[14])
                 + M[12]*(M[ 2]*M[ 7] - M[ 3]*M[ 6]));
  result[13] =   M[ 0]*(M[ 6]*M[11] - M[ 7]*M[10])
	       - M[ 4]*(M[ 2]*M[11] - M[ 3]*M[10])
	       + M[ 8]*(M[ 2]*M[ 7] - M[ 3]*M[ 6]);




  result[ 2] =   M[ 4]*(M[ 9]*M[15] - M[11]*M[13]) 
               - M[ 8]*(M[ 5]*M[15] - M[ 7]*M[13]) 
               + M[12]*(M[ 5]*M[11] - M[ 7]*M[ 9]);
  result[ 6] = -(  M[ 0]*(M[ 9]*M[15] - M[11]*M[13])
	         - M[ 8]*(M[ 1]*M[15] - M[ 3]*M[13])
	         + M[12]*(M[ 1]*M[11] - M[ 3]*M[ 9]));
  result[10] =   M[ 0]*(M[ 5]*M[15] - M[ 7]*M[13])
               - M[ 4]*(M[ 1]*M[15] - M[ 3]*M[13])
               + M[12]*(M[ 1]*M[ 7] - M[ 3]*M[ 5]);
  result[14] = -(  M[ 0]*(M[ 5]*M[11] - M[ 7]*M[ 9])
		 - M[ 4]*(M[ 1]*M[11] - M[ 3]*M[ 9])
		 + M[ 8]*(M[ 1]*M[ 7] - M[ 3]*M[ 5]));




  result[ 3] = -(  M[ 4]*(M[ 9]*M[14] - M[10]*M[13]) 
		 - M[ 8]*(M[ 5]*M[14] - M[ 6]*M[13]) 
		 + M[12]*(M[ 5]*M[10] - M[ 6]*M[ 9]));
  result[ 7] =   M[ 0]*(M[ 9]*M[14] - M[10]*M[13])
	       - M[ 8]*(M[ 1]*M[14] - M[ 2]*M[13])
	       + M[12]*(M[ 1]*M[10] - M[ 2]*M[ 9]);
  result[11] = -(  M[ 0]*(M[ 5]*M[14] - M[ 6]*M[13])
		 - M[ 4]*(M[ 1]*M[14] - M[ 2]*M[13])
		 + M[12]*(M[ 1]*M[ 6] - M[ 2]*M[ 5]));
  result[15] =   M[ 0]*(M[ 5]*M[10] - M[ 6]*M[ 9])
	       - M[ 4]*(M[ 1]*M[10] - M[ 2]*M[ 9])
	       + M[ 8]*(M[ 1]*M[ 6] - M[ 2]*M[ 5]);
}








