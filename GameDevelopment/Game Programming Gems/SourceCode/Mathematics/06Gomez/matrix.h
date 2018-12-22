/* Copyright (C) Miguel Gomez, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Miguel Gomez, 2000"
 */
#ifndef _MATRIX_H_
#define _MATRIX_H_


//
// A 3x3 matrix
//
template <class V3, class SCALAR>
struct T_MATRIX3
{
	V3 C[3];	//column vectors

	//default
	T_MATRIX3()
	{
		//identity
		C[0][0] = 1;
		C[1][1] = 1;
		C[2][2] = 1;
	}

	//initialize
	T_MATRIX3( const V3& c0, const V3& c1, const V3& c2 )
	{
		C[0] = c0;
		C[1] = c1;
		C[2] = c2;
	}

	//M[col][row]
	//indexing, read
	const V3& operator [] ( long i ) const
	{
		return C[i];
	}
	//indexing, write
	V3& operator [] ( long i )
	{
		return C[i];
	}

	//compare
	const bool operator == ( const T_MATRIX3& m ) const
	{
		return C[0]==m.C[0] && C[1]==m.C[1] && C[2]==m.C[2];
	}

	const bool operator != ( const T_MATRIX3& m ) const
	{
		return !(m == *this);
	}

	//assign
	const T_MATRIX3& operator = ( const T_MATRIX3& m )
	{
		C[0] = m.C[0];
		C[1] = m.C[1];
		C[2] = m.C[2];

		return *this;
	}

	//increment
	const T_MATRIX3& operator += ( const T_MATRIX3& m )
	{
		C[0] += m.C[0];
		C[1] += m.C[1];
		C[2] += m.C[2];

		return *this;
	}

	//decrement
	const T_MATRIX3& operator -= ( const T_MATRIX3& m ) 
	{
		C[0] -= m.C[0];
		C[1] -= m.C[1];
		C[2] -= m.C[2];

		return *this;
	}

	//self-multiply by a scalar
	const T_MATRIX3& operator *= ( const SCALAR s )
	{
		C[0] *= s;
		C[1] *= s;
		C[2] *= s;

		return *this;
	}

	//self-multiply by a matrix
	const T_MATRIX3& operator *= ( const T_MATRIX3& m )
	{
		//NOTE:  don't change the columns
		//in the middle of the operation
		T_MATRIX3 temp = (*this);

		C[0] = temp * m.C[0];
		C[1] = temp * m.C[1];
		C[2] = temp * m.C[2];

		return *this;
	}

	//self-divide by a scalar
	const T_MATRIX3& operator /= ( const SCALAR s )
	{
		C[0] /= s;
		C[1] /= s;
		C[2] /= s;

		return *this;
	}

	//add
	const T_MATRIX3 operator + ( const T_MATRIX3& m ) const
	{
		return T_MATRIX3( C[0] + m.C[0], C[1] + m.C[1], C[2] + m.C[2] );
	}

	//subtract
	const T_MATRIX3 operator - ( const T_MATRIX3& m ) const
	{
		return T_MATRIX3( C[0] - m.C[0], C[1] - m.C[1], C[2] - m.C[2] );
	}

	//divide by a scalar
	const T_MATRIX3 operator / ( const SCALAR s ) const
	{
		return T_MATRIX3( C[0]/s, C[1]/s, C[2]/s );
	}

	//post-multiply by a scalar
	const T_MATRIX3 operator * ( const SCALAR s ) const
	{
		return T_MATRIX3( C[0]*s, C[1]*s, C[2]*s );
	}

	//pre-multiply by a scalar
	friend inline const T_MATRIX3 operator * ( const SCALAR s, const T_MATRIX3& m )
	{
		return m * s;
	}

	//post-multiply by a vector
	const V3 operator * ( const V3& v ) const
	{
		return( C[0]*v.x + C[1]*v.y + C[2]*v.z );
	}

	//pre-multiply by a vector
	inline friend const V3 operator * ( const V3& v, const T_MATRIX3& m )
	{
		return V3( m.C[0].dot(v), m.C[1].dot(v), m.C[2].dot(v) );
	}

	//post-multiply by a matrix
	const T_MATRIX3 operator * ( const T_MATRIX3& m ) const
	{
		return T_MATRIX3( (*this) * m.C[0], (*this) * m.C[1], (*this) * m.C[2] );
	}

	//transpose
	T_MATRIX3 transpose() const
	{
		//turn columns on their side
		return T_MATRIX3(
						V3( C[0].x, C[1].x, C[2].x ),	//column 0
						V3( C[0].y, C[1].y, C[2].y ),	//column 1
						V3( C[0].z, C[1].z, C[2].z )	//column 2
						);
	}

	//determinant
	const SCALAR determinant() const
	{
		return C[0].dot( C[1].cross(C[2]) );
	}

	//matrix inverse
	const T_MATRIX3 inverse() const;
};



//4x4 matrix for linear transformations
template <class V4, class SCALAR>
class T_MATRIX4
{
public:

	V4 C[4];	//column vectors

public:

	T_MATRIX4()
	{
		//identity
		C[0] = V4(1,0,0,0);
		C[1] = V4(0,1,0,0);
		C[2] = V4(0,0,1,0);
		C[3] = V4(0,0,0,1);
	}

	T_MATRIX4(
				const V4& c0,
				const V4& c1,
				const V4& c2,
				const V4& c3
				)
	{
		C[0] = c0;
		C[1] = c1;
		C[2] = c2;
		C[3] = c3;
	}

	//M[col][row]

	//index a column,
	V4& operator [] ( long i )
	{
		return C[i];
	}

	//(read-only)
	const V4& operator [] ( long i ) const
	{
		return C[i];
	}

	//assign
	const T_MATRIX4& operator = ( const T_MATRIX4& m )
	{
		C[0] = m.C[0];
		C[1] = m.C[1];
		C[2] = m.C[2];
		C[3] = m.C[3];

		return *this;
	}

	//increment
	const T_MATRIX4& operator += ( const T_MATRIX4& m )
	{
		C[0] += m.C[0];
		C[1] += m.C[1];
		C[2] += m.C[2];
		C[3] += m.C[3];

		return *this;
	}

	//decrement
	const T_MATRIX4& operator -= ( const T_MATRIX4& m ) 
	{
		C[0] -= m.C[0];
		C[1] -= m.C[1];
		C[2] -= m.C[2];
		C[3] -= m.C[3];

		return *this;
	}

	//self-multiply by a scalar
	const T_MATRIX4& operator *= ( const SCALAR s )
	{
		C[0] *= s;
		C[1] *= s;
		C[2] *= s;
		C[3] *= s;

		return *this;
	}

	//self-multiply by a matrix
	const T_MATRIX4& operator *= ( const T_MATRIX4& m )
	{
		//NOTE:  don't change the columns
		//in the middle of the operation
		T_MATRIX4 temp = (*this);

		C[0] = temp * m.C[0];
		C[1] = temp * m.C[1];
		C[2] = temp * m.C[2];
		C[4] = temp * m.C[4];

		return *this;
	}

	//self-divide by a scalar
	const T_MATRIX4& operator /= ( const SCALAR s )
	{
		C[0] /= s;
		C[1] /= s;
		C[2] /= s;
		C[3] /= s;

		return *this;
	}

	//add
	const T_MATRIX4 operator + ( const T_MATRIX4& m ) const
	{
		return T_MATRIX4( C[0] + m.C[0], C[1] + m.C[1], C[2] + m.C[2], C[3] + m.C[3] );
	}

	//subtract
	const T_MATRIX4 operator - ( const T_MATRIX4& m ) const
	{
		return T_MATRIX4( C[0] - m.C[0], C[1] - m.C[1], C[2] - m.C[2], C[3] - m.C[3] );
	}

	//divide by a scalar
	const T_MATRIX4 operator / ( const SCALAR s ) const
	{
		return T_MATRIX4( C[0]/s, C[1]/s, C[2]/s, C[3]/s );
	}

	//post-multiply by a scalar
	const T_MATRIX4 operator * ( const SCALAR s ) const
	{
		return T_MATRIX4( C[0]*s, C[1]*s, C[2]*s, C[3]*s );
	}

	//pre-multiply by a scalar
	friend inline const T_MATRIX4 operator * ( const SCALAR s, const T_MATRIX4& m )
	{
		return m * s;
	}

	//post-multiply by a vector
	const V4 operator * ( const V4& v ) const
	{
		//sum up in columns
		return( C[0]*v.x + C[1]*v.y + C[2]*v.z + C[3]*v.w );
	}

	//pre-multiply by a vector
	inline friend const V4 operator * ( const V4& v, const T_MATRIX4& m )
	{
		return V4( m.C[0].dot(v), m.C[1].dot(v), m.C[2].dot(v), m.C[2].dot(v) );
	}

	//post-multiply by a matrix
	const T_MATRIX4 operator * ( const T_MATRIX4& m ) const
	{
		return T_MATRIX4( (*this) * m.C[0], (*this) * m.C[1], (*this) * m.C[2], (*this) * m.C[3] );
	}
};



// Symmetric matrices can be optimized
template< class SCALAR, class V3, class T_MATRIX3 >
class T_SYMMETRIC_MATRIX3
{
	public:

		SCALAR xx, yy, zz;	//diagonal elements
		SCALAR xy, xz, yz;	//off-diagonal elements

	public:

		T_SYMMETRIC_MATRIX3()
		{
			//identity
			xx = yy = zz = 1;
			xy = xz = yz = 0;
		}

		T_SYMMETRIC_MATRIX3(
							const SCALAR xx,
							const SCALAR yy,
							const SCALAR zz,
							const SCALAR xy,
							const SCALAR xz,
							const SCALAR yz
							)
		{
			this->xx = xx;
			this->yy = yy;
			this->zz = zz;

			this->xy = xy;
			this->xz = xz;
			this->yz = yz;
		}

		//set equal to another matrix
		const T_SYMMETRIC_MATRIX3& operator = ( const T_SYMMETRIC_MATRIX3& m )
		{
			this->xx = m.xx;
			this->yy = m.yy;
			this->zz = m.zz;

			this->xy = m.xy;
			this->xz = m.xz;
			this->yz = m.yz;

			return *this;
		}

		//increment by another matrix
		const T_SYMMETRIC_MATRIX3& operator += ( const T_SYMMETRIC_MATRIX3& m )
		{
			this->xx += m.xx;
			this->yy += m.yy;
			this->zz += m.zz;

			this->xy += m.xy;
			this->xz += m.xz;
			this->yz += m.yz;

			return *this;
		}

		//decrement by another matrix
		const T_SYMMETRIC_MATRIX3& operator -=( const T_SYMMETRIC_MATRIX3& m ) 
		{
			this->xx -= m.xx;
			this->yy -= m.yy;
			this->zz -= m.zz;

			this->xy -= m.xy;
			this->xz -= m.xz;
			this->yz -= m.yz;

			return *this;
		}

		//add two matrices
		T_SYMMETRIC_MATRIX3 operator + ( const T_SYMMETRIC_MATRIX3& m ) const
		{
			return T_SYMMETRIC_MATRIX3(
										xx + m.xx,
										yy + m.yy,
										zz + m.zz,

										xy + m.xy,
										xz + m.xz,
										yz + m.yz
										);
		}

		//subtract two matrices
		T_SYMMETRIC_MATRIX3 operator - ( const T_SYMMETRIC_MATRIX3& m ) const
		{
			return T_SYMMETRIC_MATRIX3(
										xx - m.xx,
										yy - m.yy,
										zz - m.zz,

										xy - m.xy,
										xz - m.xz,
										yz - m.yz
										);
		}


		//post-multiply by a vector
		V3 operator * ( const V3& v ) const
		{
			return V3(	v.x*xx + v.y*xy + v.z*xz,
							v.x*xy + v.y*yy + v.z*yz,
							v.z*xz + v.y*yz + v.z*zz );
		}

		//pre-multiply by a vector
		friend inline V3 operator * ( const V3& v, const T_SYMMETRIC_MATRIX3& m )
		{
			return m * v;
		}

		//NOTE:  Can't do a self-multiply because the product of two symmetric matrices
		//is not necessarily a symmetric matrix

		//multiply two symmetric matrices
//		T_SYMMETRIC_MATRIX3 operator * ( const T_SYMMETRIC_MATRIX3& m ) const
//		{
//			return T_MATRIX3();
//		}

		// - matrix specific - //
		//a symmetric matrix is equal to its transpose

		//Is there a simplified formula for the inverse of a symmetric matrix?
};


#endif
//EOF
