/*============================================================================
	Title: Sparse.cpp
	Module: Pi/MathLib
	Author: Ignacio Castaño
	Description: Naïve implementation of the Conjugate Gradient Method.
	Changes:
		22/10/2000 - Ignacio Castaño
			File added.

============================================================================*/

/*----------------------------------------------------------------------------
	Doc:
----------------------------------------------------------------------------*/

/** @file Sparse.cpp
 * Naïve implementation of the Conjugate Gradient Method.
**/


/*----------------------------------------------------------------------------
	Includes:
----------------------------------------------------------------------------*/

#include "Sparse.h"



/*----------------------------------------------------------------------------
	Functions:
----------------------------------------------------------------------------*/

/**
 * Compute the solution of the sparse linear system Ab=x using the Conjugate
 * Gradient method.
 *
 * Solving sparse linear systems:
 * (1)		A·x = b
 * 
 * The conjugate gradient algorithm solves (1) only in the case that A is 
 * symmetric and positive definite. It is based on the idea of minimizing the 
 * function
 * 
 * (2)		f(x) = 1/2·x·A·x - b·x
 * 
 * This function is minimized when its gradient
 * 
 * (3)		df = A·x - b
 * 
 * is zero, which is equivalent to (1). The minimization is carried out by 
 * generating a succession of search directions p.k and improved minimizers x.k. 
 * At each stage a quantity alfa.k is found that minimizes f(x.k + alfa.k·p.k), 
 * and x.k+1 is set equal to the new point x.k + alfa.k·p.k. The p.k and x.k are 
 * built up in such a way that x.k+1 is also the minimizer of f over the whole
 * vector space of directions already taken, {p.1, p.2, . . . , p.k}. After N 
 * iterations you arrive at the minimizer over the entire vector space, i.e., the 
 * solution to (1).
 *
 * For a really good explanation of the method see:
 *
 * "An Introduction to the Conjugate Gradient Method Without the Agonizing Pain",
 * Jonhathan Richard Shewchuk.
 *
**/
int ConjugateGradientSolve( SparseMatrix &A, SparseVector &b, SparseVector &x, REAL error ) {
	piLightCheck( A.Dim() == b.Dim() );
	piLightCheck( A.Dim() == x.Dim() );

	int i = 0;
	const int D = A.Dim();
	const int i_max = D;

	SparseVector r(D);		// residual
	SparseVector d(D);		// search direction
	SparseVector q(D);		// 
	SparseVector tmp(D);	// temporal vector.
	REAL delta_0;
	REAL delta_old;
	REAL delta_new;
	REAL alfa;
	REAL beta;

	// r = b - A·x;
	A.Product( x, tmp );
	r.Sub( b, tmp );

	// d = r;
	d.Set( r );
	
	delta_new = SparseVectorDotProduct( r, r );
	delta_0 = delta_new;

	while( i<i_max && delta_new > error*error*delta_0 ) {

		i++;

		// q = A·d
		A.Product( d, q );

		// alfa = delta_new / d·q
		alfa = delta_new / SparseVectorDotProduct( d, q );

		// x = x + alfa·d
		x.Mad( x, d, alfa );

		if( i&31 == 0 ) {			// recompute r after 32 steps
			// r = b - A·x
			A.Product( x, tmp );
			r.Sub( b, tmp );
		}
		else {
			// r = r - alfa·q
			r.Mad( r, q, -alfa );
		}

		delta_old = delta_new;
		delta_new = SparseVectorDotProduct( r, r );

		beta = delta_new / delta_old;

		// d = r + beta·d
		d.Mad( r, d, beta );

	}


	return i;
}


