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
#include "mtxlib.h"
#include <math.h>


typedef vector4 quaternion;

const int kX = 0;
const int kY = 1;
const int kZ = 2;
const int kW = 3;

/*
Convert a quaternion (q) to a 4x4 matrix (m)
*/
void QuaternionToMatrix(const quaternion &q,matrix44 &m)
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
	m[kY][kX] = xy + wz;
	m[kZ][kX] = xz - wy;
	
	/*
	y axis
	*/
	m[kX][kY] = xy - wz;
	m[kY][kY] = 1.0 - (xx + zz);
	m[kZ][kY] = yz + wx;
	
	/*
	z axis
	*/
	m[kX][kZ] = xz + wy;
	m[kY][kZ] = yz - wx;
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

void MatrixToQuaternion(const matrix44 &m,quaternion &q)
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
		q[kX] = (m[kZ][kY] - m[kY][kZ])/fourD;
		q[kY] = (m[kX][kZ] - m[kZ][kX])/fourD;
		q[kZ] = (m[kY][kX] - m[kX][kY])/fourD;
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
		q[kW] = (m[k][j] - m[j][k])/fourD;
	}
}

