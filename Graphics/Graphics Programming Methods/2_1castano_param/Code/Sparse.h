/*============================================================================
	Title: Sparse.h
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: Sparse matrix manipulation code.
	Changes:
		22/10/2000 - Ignacio Castaño
			File added.

============================================================================*/

#ifndef _PI_SPARSE_H_
#define _PI_SPARSE_H_

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file Sparse.h
 * Sparse matrix manipulation code.
**/


/*----------------------------------------------------------------------------
	Headers:
----------------------------------------------------------------------------*/

//#include "Core.h"
//#include "MathLib.h"
#include "Containers.h"

#include "PAR.h"



/*----------------------------------------------------------------------------
	Classes:
----------------------------------------------------------------------------*/

/**
 * The sparse vector is not 'sparse'. It's only a big vector to be used in 
 * conjunction with the SparseMatrix class. The size of the vector is 
 * constant, you cannot grow or shrink it, but it's not implemented as a
 * template, for the sake of simplicity and to avoid 'code bloat'.
**/
class MATHLIB_API SparseVector {
	PI_DISABLE_COPY(SparseVector);
public:

	/** Ctor. Init dimension of the vector. */
	SparseVector( int dim ) : array(dim) { }


	/** Return vector dimension. */
	int Dim( void ) const { return array.Size(); }


	/** Const and save vector access. */
	const REAL & operator[]( int index ) const { return array[index]; }
	

	/** Safe vector access. */
	REAL & operator[] ( int index ) { return array[index]; }


	/** Vector copy. */
	void Zero( void ) {
		const int D = Dim();
		for( int i=0; i<D; i++ )
			array[i] = 0;
	}


	/** Vector copy. */
	void Set( const SparseVector &a ) {
		const int D = Dim();
		for( int i=0; i<D; i++ )
			array[i] = a[i];
	}


	/** Vector substraction. */
	void Sub( const SparseVector &a, const SparseVector &b ) {
		const int D = Dim();
		for( int i=0; i<D; i++ )
			array[i] = a[i] - b[i];
	}


	/** Vector addition. */
	void Add( const SparseVector &a, const SparseVector &b ) {
		const int D = Dim();
		for( int i=0; i<D; i++ )
			array[i] = a[i] + b[i];
	}


	/** Vector 'modulate and add' (that is: this = a + b·s). */
	void Mad( const SparseVector &a, const SparseVector &b, REAL s ) {
		const int D = Dim();
		for( int i=0; i<D; i++ )
			array[i] = a[i] + b[i] * s;
	}

	// Friends:
	friend REAL SparseVectorDotProduct( const SparseVector &a, const SparseVector &b );


protected:

	PiArray<REAL>	array;
};



/**
 * Sparse matrix class. The matrix is assumed to be sparse and to have
 * very few non-zero elements, for this reason it's stored in indexed 
 * format. To multiply column vectors efficiently, the matrix stores 
 * the elements in indexed-column order, there is a list of indexed 
 * elements for each row of the matrix. As with the SparseVector the 
 * dimension of the matrix is constant.
**/
class MATHLIB_API SparseMatrix {
	PI_DISABLE_COPY(SparseMatrix);
public:

	/** Ctor. Init the size of the sparse matrix. */
	SparseMatrix( int size ) : array(size) {
	}

	/** Set an element of the sparse matrix. */
	void SetElem( int x, int y, REAL v ) {
		piLightCheck( x>=0 && x<array.Size() );
		piLightCheck( y>=0 && y<array.Size() );

		Elem e = { x, v };
		array[y].PushBack( e );
	}


	/** Set an element of the sparse matrix. */
	REAL GetElem( int x, int y ) {
		piLightCheck( x>=0 && x<array.Size() );
		piLightCheck( y>=0 && y<array.Size() );

		for( int i=0; i<array[y].Size(); i++ )
			if( array[y][i].x == x ) return array[y][i].v;
		return 0;
	}

	/** Get the dimension of the matrix. */
	int Dim( void ) const { 
		return array.Size();
	}


	/** 
	 * Compute the product of the vector @a src and this matrix. That is: A·src = dst. 
	 * @a src is a column vector.
	**/
	void Product( const SparseVector & src, SparseVector & dst ) {
		int D = Dim();
		piLightCheck( D == src.Dim() );
		piLightCheck( D == dst.Dim() );

		for( int i=0; i<D; i++ ) {
			REAL dot = 0;
			for( int e=0; e<array[i].Size(); e++ ) {
				int x = array[i][e].x;
				REAL v = array[i][e].v;

				dot += v * src[x];
			}
			dst[i] = dot;
		}
	}


	/** Add the elements of the given column. */
	float AddColumn( int i ) {
		REAL add = 0;
		for( int e=0; e<array[i].Size(); e++ )
			add += array[i][e].v;

		return add;
	}


	// Friends.
	MATHLIB_API friend int ConjugateGradientSolve( SparseMatrix &A, SparseVector &b, SparseVector &x );



protected:

	struct Elem {
		int		x;	// column
		REAL	v;	// value
	};

	PiArray< PiArray<Elem> >	array;

};



/*----------------------------------------------------------------------------
	Functions:
----------------------------------------------------------------------------*/

/**
 * Sparse vector dot product.
**/
inline REAL SparseVectorDotProduct( const SparseVector &a, const SparseVector &b ) {
	piLightCheck( a.Dim() == b.Dim() );
	REAL dot = 0;
	int D = a.Dim();
	for( int i=0; i<D; i++ )
		dot += a[i] * b[i];
	return dot;
}


/*
 * Compute the solution of the sparse linear system Ab=x using the Conjugate
 * Gradient method.
 */
MATHLIB_API int ConjugateGradientSolve( SparseMatrix &A, SparseVector &b, SparseVector &x, REAL epsilon=0.00001 );





#endif // _PI_SPARSE_H_
