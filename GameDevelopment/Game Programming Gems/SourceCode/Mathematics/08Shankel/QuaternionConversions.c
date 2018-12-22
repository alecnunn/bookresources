/* Copyright (C) Jason Shankel, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jason Shankel, 2000"
 */
/*
	Matrix-Quaternion Conversions
*/
#include <math.h>


const int kX = 0;
const int kY = 1;
const int kZ = 2;
const int kW = 3;

/*
Quaternion
*/
typedef double Quaternion_t [4];

/*
4x4 matrix.  Matrix packing order to be in OpenGL-compatible format 
(column data packed contiguously)
*/
typedef double Matrix_t [4][4];


/*
Convert a quaternion (q) to a 4x4 matrix (m)
*/
void QuaternionToMatrix(Quaternion_t q,Matrix_t m)
{
	/*
	If q is guaranteed to be a unit quaternion, s will always
	be 1.  In that case, this calculation can be optimized out.
	*/
	double
	norm = q[kX]*q[kX] + q[kY]*q[kY] + q[kZ]*q[kZ] + q[kW]*q[kW],
	s = (norm > 0) ? 2/norm : 0,
	
	/*
	Precalculate coordinate products
	*/
	xx = q[kX] * q[kX] * s,
	yy = q[kY] * q[kY] * s,
	zz = q[kZ] * q[kZ] * s,
	xy = q[kX] * q[kY] * s,
	xz = q[kX] * q[kZ] * s,
	yz = q[kY] * q[kZ] * s,
	wx = q[kW] * q[kX] * s,
	wy = q[kW] * q[kY] * s,
	wz = q[kW] * q[kZ] * s;

	/*
	Calculate 3x3 matrix from orthonormal basis
	*/

	/*
	x axis
	*/
	m[kX][kX] = 1.0 - (yy + zz);
	m[kX][kY] = xy + wz;
	m[kX][kZ] = xz - wy;
	
	/*
	y axis
	*/
	m[kY][kX] = xy - wz;
	m[kY][kY] = 1.0 - (xx + zz);
	m[kY][kZ] = yz + wx;
	
	/*
	z axis
	*/
	m[kZ][kX] = xz + wy;
	m[kZ][kY] = yz - wx;
	m[kZ][kZ] = 1.0 - (xx + yy);

	/*
	4th row and column of 4x4 matrix
	Translation and scale are not stored in quaternions, so these
	values are set to default (no scale, no translation).
	For systems where m comes pre-loaded with scale and translation
	factors, this code can be excluded.
	*/
	m[kW][kX] = m[kW][kY] = m[kW][kZ] = m[kX][kW] = m[kY][kW] = m[kZ][kW] = 0.0;
	m[kW][kW] = 1.0;

}

void MatrixToQuaternion(Matrix_t m,Quaternion_t q)
{
	/*
	This code can be optimized for m[kW][kW] = 1, which 
	should always be true.  This optimization is excluded
	here for clarity.
	*/
	
	double Tr = m[kX][kX] + m[kY][kY] + m[kZ][kZ] + m[kW][kW],fourD;
	int i,j,k;
	
	/*
	w >= 0.5 ?
	*/
	if (Tr >= 1.0)
	{
		fourD = 2.0*sqrt(Tr);
		q[kW] = fourD/4.0;
		q[kX] = (m[kY][kZ] - m[kZ][kY])/fourD;
		q[kY] = (m[kZ][kX] - m[kX][kZ])/fourD;
		q[kZ] = (m[kX][kY] - m[kY][kX])/fourD;
	}
	else
	{
		/*
		Find the largest component.  
		*/
		if (m[kX][kX] > m[kY][kY])
		{
			i = kX;
		}
		else
		{
			i = kY;
		}
		if (m[kZ][kZ] > m[i][i])
		{
			i = kZ;
		}

		
		/*
		Set j and k to point to the next two components
		*/
		j = (i+1)%3;
		k = (j+1)%3;

		/*
		fourD = 4 * largest component
		*/
		fourD = 2.0*sqrt(m[i][i] - m[j][j] - m[k][k] + 1.0 );

		/*
		Set the largest component
		*/
		q[i] = fourD/4.0;
		
		/*
		Calculate remaining components
		*/
		q[j]  = (m[j][i] + m[i][j])/fourD;
		q[k]  = (m[k][i] + m[i][k])/fourD;
		q[kW] = (m[j][k] - m[k][j])/fourD;
	}
}

