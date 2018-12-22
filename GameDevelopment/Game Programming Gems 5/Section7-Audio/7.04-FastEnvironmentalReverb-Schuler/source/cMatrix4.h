
#ifndef MATRIX4_INCLUDED
#define MATRIX4_INCLUDED


#include <complex>

/**
A 4x4 vector class (row vectors).
Defines the ops necessary for calculation of the FDN transfer function.
**/

template< typename T >
class cVector4
{
public:
	T elements[4];

public:
	cVector4() {}

	cVector4 &operator =( const float [4] );

	operator T *() { return elements; }
	operator const T *() const { return elements; }
};

template< typename T >
cVector4<T> &cVector4<T>::operator =( const float other[4] )
{
	elements[0] = other[0];
	elements[1] = other[1];
	elements[2] = other[2];
	elements[3] = other[3];
	return *this;
}


template< typename T >
cVector4<T> operator +( const cVector4<T> &A, const cVector4<T> &B )
{
	cVector4<T> result;
	result[0] = A[0] + B[0];
	result[1] = A[1] + B[1];
	result[2] = A[2] + B[2];
	result[3] = A[3] + B[3];
	return result;
}


template< typename T >
cVector4<T> operator -( const cVector4<T> &A, const cVector4<T> &B )
{
	cVector4<T> result;
	result[0] = A[0] - B[0];
	result[1] = A[1] - B[1];
	result[2] = A[2] - B[2];
	result[3] = A[3] - B[3];
	return result;
}



template< typename T >
cVector4<T> operator *( float A, const cVector4<T> &B )
{
	cVector4<T> result;
	result[0] = A * B[0];
	result[1] = A * B[1];
	result[2] = A * B[2];
	result[3] = A * B[3];
	return result;
}



template< typename T >
T dot( const cVector4<T> &A, const cVector4<T> &B )
{
	return A[0] * B[0] + A[1] * B[1] + A[2] * B[2] + A[3] * B[3];
}



/**
A 4x4 matrix class.
Defines the ops necessary for calculation of the FDN transfer function.
**/

template< typename T >
class cMatrix4
{
public:
	
	cMatrix4() {}
	
	cMatrix4 &operator =( const float [4][4] );
	
	cVector4<T> rows[4];

	void zero();
	void identity();
	T adjoint( unsigned, unsigned ) const;
	T determinant() const;

public:
	operator cVector4<T> *() { return rows; }
	operator const cVector4<T> *() const { return rows; }
};


template< typename T >
cMatrix4<T> &cMatrix4<T>::operator =( const float other[4][4] )
{
	for( unsigned i = 0; i < 4; i++ )
		for( unsigned j = 0; j < 4; j++ )
			rows[i][j] = other[i][j];

	return *this;
}


template< typename T >
void cMatrix4<T>::zero()
{
	for( unsigned i = 0; i < 4; i++ )
		for( unsigned j = 0; j < 4; j++ )
			rows[i][j] = 0;
}



template< typename T >
void cMatrix4<T>::identity()
{
	zero();
	rows[0][0] = T(1);
	rows[1][1] = T(1);
	rows[2][2] = T(1);
	rows[3][3] = T(1);
}


template< typename T >
T cMatrix4<T>::adjoint( unsigned row, unsigned col ) const
{
	// return determinant of 3x3 submatrix 

	unsigned a = ( row + 1 ) & 3;
	unsigned b = ( row + 2 ) & 3;
	unsigned c = ( row + 3 ) & 3;

	unsigned x = ( col + 1 ) & 3;
	unsigned y = ( col + 2 ) & 3;
	unsigned z = ( col + 3 ) & 3;

	T result = 0;
	result += rows[a][x] * ( rows[b][y] * rows[c][z] - rows[b][z] * rows[c][y] );
	result += rows[a][y] * ( rows[b][z] * rows[c][x] - rows[b][x] * rows[c][z] );
	result += rows[a][z] * ( rows[b][x] * rows[c][y] - rows[b][y] * rows[c][x] );

	// check sign

	return ( row ^ col ) & 1 ? -1.f * result : result;
}


template< typename T >
T cMatrix4<T>::determinant() const
{
	return
		rows[0][0] * adjoint( 0, 0 ) +
		rows[0][1] * adjoint( 0, 1 ) +
		rows[0][2] * adjoint( 0, 2 ) +
		rows[0][3] * adjoint( 0, 3 );
}



template< typename T >
cMatrix4<T> operator +( const cMatrix4<T> &A, const cMatrix4<T> &B )
{
	cMatrix4<T> result;

	result[0] = A[0] + B[0];
	result[1] = A[1] + B[1];
	result[2] = A[2] + B[2];
	result[3] = A[3] + B[3];

	return result;
}


template< typename T >
cMatrix4<T> operator -( const cMatrix4<T> &A, const cMatrix4<T> &B )
{
	cMatrix4<T> result;

	result[0] = A[0] - B[0];
	result[1] = A[1] - B[1];
	result[2] = A[2] - B[2];
	result[3] = A[3] - B[3];


	return result;
}



template< typename T >
cMatrix4<T> operator *( float A, const cMatrix4<T> &B )
{
	cMatrix4<T> result;

	result[0] = A * B[0];
	result[1] = A * B[1];
	result[2] = A * B[2];
	result[3] = A * B[3];

	return result;
}

template< typename T >
cMatrix4<T> inverse( const cMatrix4<T> &A )
{
	cMatrix4<T> result;
	T invdet = T(1) / A.determinant();
	
	for( unsigned i = 0; i < 4; i++ )
		for( unsigned j = 0; j < 4; j++ )
			result[i][j] = invdet * A.adjoint( j, i );

	return result;
}


template< typename T >
cMatrix4<T> mul( const cMatrix4<T> &A, const cMatrix4<T> &B )
{
	cMatrix4<T> result;

	for( unsigned i = 0; i < 4; i++ )
		for( unsigned j = 0; j < 4; j++ )
		{
			result.rows[i][j] = 
				A[i][0] * B[0][j] + 
				A[i][1] * B[1][j] + 
				A[i][2] * B[2][j] + 
				A[i][3] * B[3][j];
		}

	return result;
}

template< typename T >
cVector4<T> mul( const cVector4<T> &A, const cMatrix4<T> &B )
{
	cVector4<T> result;

	for( unsigned j = 0; j < 4; j++ )
	{
		result[j] =
			A[0] * B[0][j] + 
			A[1] * B[1][j] + 
			A[2] * B[2][j] + 
			A[3] * B[3][j];
	}

	return result;
}



// typedef used for the rest of the source code
typedef cVector4< std::complex< float > > cComplexVector4;
typedef cMatrix4< std::complex< float > > cComplexMatrix4;

#endif
