//==============================================================
//= Matrix.h
//= Original coders: Trent Polack (trent@codershq.com) and
//-					 Evan Pipho	  (evan@codershq.com)
//==============================================================
//= A 4x4 matrix class
//==============================================================
#ifndef __MathLib_Matrix_H__
#define __MathLib_Matrix_H__

namespace MathLib {
	//a 4x4 matrix
	class Matrix {

	public:
		float m_matrix[16];

		void Set( float *pMatrix );

		void SetIdentity( void );

		void MultiplyByMatrix( Matrix& pMtrx );
		Matrix MultiplyMatrix( Matrix& pMtrx );

		void SetTranslation( float *pVec );
		void SetInvTranslation( float *pVec );
		void InvTranslateVec( float *pVec );

		void SetRotation( float *pVec );
		void SetInvRotation( float *pVec );
		void InvRotateVec( float *pVec );

		//-
		//get the matrix's information (16 entry floating-point array)
		inline float *Get( void ) {
			return m_matrix;
		}

	Matrix( void ) { }
	~Matrix( void ) { }
	};
}

#endif	//__MathLib_Matrix_H__