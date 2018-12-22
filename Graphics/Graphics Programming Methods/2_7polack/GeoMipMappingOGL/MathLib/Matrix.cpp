//==============================================================
//= Matrix.h
//= Original coders: Trent Polack (trent@codershq.com) and
//-					 Evan Pipho	  (evan@codershq.com)
//==============================================================
//= A 4x4 matrix class
//==============================================================
#include <cmath>
#include <memory.h>

#include "Matrix.h"
using namespace MathLib;

//set the matrix's information
void Matrix::Set( float *pMatrix ) {
	memcpy( pMatrix, pMatrix, sizeof( float[16] ) );
}

//set the matrix's information to that of the identity matrix
void Matrix::SetIdentity( void ) {
	memset( m_matrix, 0, sizeof( float[16] ) );

	m_matrix[0]= m_matrix[5]= m_matrix[10]= m_matrix[15] = 1;
}

//multiply the class matrix by another
void Matrix::MultiplyByMatrix( Matrix& pMat ) {
	const float *m1= m_matrix;
	const float *m2= pMat.Get( );
	float newMtrx[16];

	newMtrx[0] = m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2] + m1[12]*m2[3];
	newMtrx[1] = m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2] + m1[13]*m2[3];
	newMtrx[2] = m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	newMtrx[3] = m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

	newMtrx[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6] + m1[12]*m2[7];
	newMtrx[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6] + m1[13]*m2[7];
	newMtrx[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	newMtrx[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

	newMtrx[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10] + m1[12]*m2[11];
	newMtrx[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10] + m1[13]*m2[11];
	newMtrx[10]= m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	newMtrx[11]= m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

	newMtrx[12]= m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14] + m1[12]*m2[15];
	newMtrx[13]= m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14] + m1[13]*m2[15];
	newMtrx[14]= m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	newMtrx[15]= m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];

	Set( newMtrx );
}

//matrix multiplication function
Matrix Matrix::MultiplyMatrix( Matrix& pMtrx ) {
	const float *m1= m_matrix;
	const float *m2= pMtrx.Get( );
	Matrix newMtrx;

	newMtrx.m_matrix[0]= m1[0]*m2[0] + m1[4]*m2[1] + m1[8]*m2[2]  + m1[12]*m2[3];
	newMtrx.m_matrix[1]= m1[1]*m2[0] + m1[5]*m2[1] + m1[9]*m2[2]  + m1[13]*m2[3];
	newMtrx.m_matrix[2]= m1[2]*m2[0] + m1[6]*m2[1] + m1[10]*m2[2] + m1[14]*m2[3];
	newMtrx.m_matrix[3]= m1[3]*m2[0] + m1[7]*m2[1] + m1[11]*m2[2] + m1[15]*m2[3];

	newMtrx.m_matrix[4] = m1[0]*m2[4] + m1[4]*m2[5] + m1[8]*m2[6]  + m1[12]*m2[7];
	newMtrx.m_matrix[5] = m1[1]*m2[4] + m1[5]*m2[5] + m1[9]*m2[6]  + m1[13]*m2[7];
	newMtrx.m_matrix[6] = m1[2]*m2[4] + m1[6]*m2[5] + m1[10]*m2[6] + m1[14]*m2[7];
	newMtrx.m_matrix[7] = m1[3]*m2[4] + m1[7]*m2[5] + m1[11]*m2[6] + m1[15]*m2[7];

	newMtrx.m_matrix[8] = m1[0]*m2[8] + m1[4]*m2[9] + m1[8]*m2[10]  + m1[12]*m2[11];
	newMtrx.m_matrix[9] = m1[1]*m2[8] + m1[5]*m2[9] + m1[9]*m2[10]  + m1[13]*m2[11];
	newMtrx.m_matrix[10]= m1[2]*m2[8] + m1[6]*m2[9] + m1[10]*m2[10] + m1[14]*m2[11];
	newMtrx.m_matrix[11]= m1[3]*m2[8] + m1[7]*m2[9] + m1[11]*m2[10] + m1[15]*m2[11];

	newMtrx.m_matrix[12]= m1[0]*m2[12] + m1[4]*m2[13] + m1[8]*m2[14]  + m1[12]*m2[15];
	newMtrx.m_matrix[13]= m1[1]*m2[12] + m1[5]*m2[13] + m1[9]*m2[14]  + m1[13]*m2[15];
	newMtrx.m_matrix[14]= m1[2]*m2[12] + m1[6]*m2[13] + m1[10]*m2[14] + m1[14]*m2[15];
	newMtrx.m_matrix[15]= m1[3]*m2[12] + m1[7]*m2[13] + m1[11]*m2[14] + m1[15]*m2[15];

	return newMtrx;
}

//set the matrix's translation via a 3D vector
void Matrix::SetTranslation( float *pVec ) {
	m_matrix[12]= pVec[0];
	m_matrix[13]= pVec[1];
	m_matrix[14]= pVec[2];
	m_matrix[15]= 1;
}

//set the matrix's inverse translation via a 3D vector
void Matrix::SetInvTranslation( float *pVec ) {
	m_matrix[12]= -pVec[0];
	m_matrix[13]= -pVec[1];
	m_matrix[14]= -pVec[2];
	m_matrix[15]= 1;
}

//inversely translate the matrix via a 3D vector
void Matrix::InvTranslateVec( float *pVec ) {
	pVec[0]= pVec[0] - m_matrix[12];
	pVec[1]= pVec[1] - m_matrix[13];
	pVec[2]= pVec[2] - m_matrix[14];
}

//set the matrix's rotation via a 3D vector
void Matrix::SetRotation( float *pVec ) {
	float CX= cosf( pVec[0] );
	float SX= sinf( pVec[0] );
	float CY= cosf( pVec[1] );
	float SY= sinf( pVec[1] );
	float CZ= cosf( pVec[2] );
	float SZ= sinf( pVec[2] );

	m_matrix[0] = CY * CZ;
	m_matrix[1] = CY * SZ;
	m_matrix[2] =-SY;

	m_matrix[4] = SX * SY * CZ - CX * SZ;
	m_matrix[5] = SX * SY * SZ + CX * CZ;
	m_matrix[6] = SX * CY;

	m_matrix[8] = CX * SY * CZ + SX * SZ;
	m_matrix[9] = CX * SY * SZ - SX * CZ;
	m_matrix[10]= CX * CY;

	m_matrix[15]= 1;
}

//set the matrix's inverse rotation via a 3D vector
void Matrix::SetInvRotation( float *pVec ) {
	float CX= cosf( pVec[0] );
	float SX= sinf( pVec[0] );
	float CY= cosf( pVec[1] );
	float SY= sinf( pVec[1] );
	float CZ= cosf( pVec[2] );
	float SZ= sinf( pVec[2] );

	m_matrix[0] = CY * CZ;
	m_matrix[4] = CY * SZ;
	m_matrix[8] =-SY;

	m_matrix[1] = SX * SY * CZ - CX * SZ;
	m_matrix[5] = SX * SY * SZ + CX * CZ;
	m_matrix[9] = SX * CY;

	m_matrix[2] = CX * SY * CZ + SX * SZ;
	m_matrix[6] = CX * SY * SZ - SX * CZ;
	m_matrix[10]= CX * CY;

	m_matrix[15]= 1;
}

//inversely rotate the matrix via a 3D vector
void Matrix::InvRotateVec( float *pVec ) {
	float fVec[3];
	
	fVec[0]= pVec[0]*m_matrix[0] +
			 pVec[1]*m_matrix[1] +
			 pVec[2]*m_matrix[2];

	fVec[1]= pVec[0]*m_matrix[4] +
			 pVec[1]*m_matrix[5] +
			 pVec[2]*m_matrix[6];

	fVec[2]= pVec[0]*m_matrix[8] +
			 pVec[1]*m_matrix[9] +
			 pVec[2]*m_matrix[10];

	//copy the results from the calculations
	memcpy( pVec, fVec, sizeof( float[3] ) );
}
