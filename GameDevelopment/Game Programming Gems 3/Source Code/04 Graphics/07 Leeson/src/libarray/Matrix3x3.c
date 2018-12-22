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
 * Date: $Date: 2001/08/23 16:44:08 $                                       *
 * Author: $Author: leesonw $                                               *
 * Version: $Revision: 1.3 $	                                            *
 * Description: This provides a set of functions to handle various          *
 *              operations performed on 3x3 matrices.                       *
\****************************************************************************/
#include "Matrix3x3.h"

/*
 *                             Matrix Addition
 */
void MAdd3x3(const real *op1,const real *op2,real *result)
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
}

/*
 *                             Matrix Subtraction
 */
void MSubtract3x3(const real *op1,const real *op2,real *result)
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
}

/*
 *                             Matrix Negation
 */
void MNegate3x3(const real *op,real *result)
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
}

/*
 *                             Matrix Multiplication
 */
void MMMultiply3x3(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[3]*op2[1] + op1[6]*op2[2];
  result[1] = op1[1]*op2[0] + op1[4]*op2[1] + op1[7]*op2[2];
  result[2] = op1[2]*op2[0] + op1[5]*op2[1] + op1[8]*op2[2];

  result[3] = op1[0]*op2[3] + op1[3]*op2[4] + op1[6]*op2[5];
  result[4] = op1[1]*op2[3] + op1[4]*op2[4] + op1[7]*op2[5];
  result[5] = op1[2]*op2[3] + op1[5]*op2[4] + op1[8]*op2[5];

  result[6] = op1[0]*op2[6] + op1[3]*op2[7] + op1[6]*op2[8];
  result[7] = op1[1]*op2[6] + op1[4]*op2[7] + op1[7]*op2[8];
  result[8] = op1[2]*op2[6] + op1[5]*op2[7] + op1[8]*op2[8];
}

/*
 *                       Vector Matrix Multiplication
 */
void MVMultiply3x3(const real *op1,const real *op2,real *result)
{
  result[0] = op1[0]*op2[0] + op1[3]*op2[1] + op1[6]*op2[2];
  result[1] = op1[1]*op2[0] + op1[4]*op2[1] + op1[7]*op2[2];
  result[2] = op1[2]*op2[0] + op1[5]*op2[1] + op1[8]*op2[2];
}

/*
 *   Transpose a matrix
 */
void MTranspose3x3(const real *op,real *result)
{
  result[0] = op[0];
  result[1] = op[3];
  result[2] = op[6];

  result[3] = op[1];
  result[4] = op[4];
  result[5] = op[7];

  result[6] = op[2];
  result[7] = op[5];
  result[8] = op[8];
}

/*
 *		Matrix by a scalar
 */
void MSMultiply3x3(real op1,const real *op2,real *result)
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
}

/*
 *		Matrix divided by a scalar
 */
void MSDivide3x3(real op1,const real *op2,real *result)
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
}

/*
 *  Invert a Matrix
 */
bool MInvert3x3(const real *operand,real *result)
{
  integer i,j,k;
  real tmp_mat[3*3];
  real tmp_d;

  memcpy(tmp_mat,operand,sizeof(real)*3*3);

  // Unit Matrix
  MIdentity3x3(result);

  for(i = 0; i < 3; i++)
    {
      for(j = i + 1, k = i; j < 3; j++)
        {
	  if(fabs(tmp_mat[j + i*3]) > fabs(tmp_mat[k + i*3]))
	    {
	      k = j;
	    }
        }
      /* check for singularity */
      if(tmp_mat[k + i*3] == 0.0)
	{
	  return false;
	}
      /* pivot - switch rows k and i */
      if(k != i)
	{
	  for(j = 0;j < 3; j++)
            {
	      tmp_d = tmp_mat[i + j*3];
	      tmp_mat[i + j*3] = tmp_mat[k + j*3];
	      tmp_mat[k + j*3] = tmp_d;
	      tmp_d = result[i + j*3];
	      result[i + j*3] = result[k + j*3];
	      result[k + j*3] = tmp_d;
            }
	}
      /* normalize the row - make the diagonal 1 */
      for(tmp_d = 1.0/tmp_mat[i + i*3],j = 0;j < 3;j++)
        {
	  tmp_mat[i + j*3] = tmp_mat[i + j*3]*tmp_d;
	  result[i + j*3] = result[i + j*3]*tmp_d;
        }
      /* zero the non-diagonal terms in this column */
      for(j = 0;j < 3;j++)
        {
	  if(j != i)
	    {
	      for(tmp_d = -tmp_mat[j + i*3],k = 0;k < 3;k++)
		{
		  tmp_mat[j + k*3] = tmp_mat[j + k*3] + tmp_mat[i + k*3]*tmp_d;
		  result[j + k*3] = result[j + k*3] + result[i + k*3]*tmp_d;
		}
	    }
        }
    }
  return true;
}

/*
 *MPrint3x3
 */
void MPrint3x3(const real *op)
{
 fprintf(stderr,"| % f % f % f |\n",op[0] ,op[3] ,op[6]);
 fprintf(stderr,"| % f % f % f |\n",op[1] ,op[4] ,op[7]);
 fprintf(stderr,"| % f % f % f |\n",op[2] ,op[5] ,op[8]);
}

/*
 * Identity matrix
 */
void MIdentity3x3(real *result)
{
  
  result[0] = 1.0;
  result[1] = 0.0;
  result[2] = 0.0;

  result[3] = 0.0;
  result[4] = 1.0;
  result[5] = 0.0;

  result[6] = 0.0;
  result[7] = 0.0;
  result[8] = 1.0;
}

/*
 * Create a Rotation Matrix
 */
/*
 * Rotate around an axis
 */
void MRotate3x3(real angle,real *p_axis,real *M)
{
 real c,s;

 c = cos(angle);
 s = sin(angle);

 M[0] = p_axis[0]*p_axis[0]*(1 - c) + c;           M[3] = p_axis[0]*p_axis[1]*(1 - c) - p_axis[2]*s; M[6] = p_axis[0]*p_axis[2]*(1 - c) + p_axis[1]*s;
 M[1] = p_axis[1]*p_axis[0]*(1 - c) + p_axis[2]*s; M[4] = p_axis[1]*p_axis[1]*(1 - c) + c;           M[7] = p_axis[1]*p_axis[2]*(1 - c) - p_axis[0]*s;
 M[2] = p_axis[0]*p_axis[2]*(1 - c) - p_axis[1]*s; M[5] = p_axis[1]*p_axis[2]*(1 - c) + p_axis[0]*s; M[8] = p_axis[2]*p_axis[2]*(1 - c) + c;
}

/*
 *  Determinant
 */
real MDeterminant3x3(const real *op)
{
  return op[0]*(op[4]*op[8] - op[7]*op[5]) 
       - op[3]*(op[1]*op[8] - op[7]*op[2]) 
       + op[6]*(op[1]*op[5] - op[2]*op[4]);
}

/*
 * Adjoint
 */
void MAdjoint3x3(const real *M,real *result)
{
  result[0] =   M[4]*M[8] - M[5]*M[7];
  result[1] = -(M[3]*M[8] - M[5]*M[6]);
  result[2] =   M[3]*M[7] - M[4]*M[6];

  result[3] = -(M[1]*M[8] - M[2]*M[7]);
  result[4] =   M[0]*M[8] - M[2]*M[6];
  result[5] = -(M[3]*M[7] - M[4]*M[6]);

  result[6] =   M[1]*M[5] - M[2]*M[4];
  result[7] = -(M[0]*M[5] - M[2]*M[3]);
  result[8] =   M[0]*M[4] - M[1]*M[3];
}



