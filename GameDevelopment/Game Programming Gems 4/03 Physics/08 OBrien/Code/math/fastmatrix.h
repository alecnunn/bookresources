/*********************************************************************
 * fastmatrix.h
 * Authored by Kris Hauser 2002-2003
 *
 * Declares matrix_type (an efficient C-style matrix)
 * and several inlined helper functions.
 *
 * Copyright (c) 2002 SquireSoft, used with permission.
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/


#ifndef FAST_MATRIX_H
#define FAST_MATRIX_H

#include <stdlib.h>
#include <stdio.h>

#include "fastvector.h"

typedef Real** matrix_type;
/****************************************************
  The layout of matrix type is row major, starting with an array of m pointers,
  pointing to a contiguous block of m*n reals stored "bitmap-style".  This block
  directly follows the pointers.
  i.e.
  mat -> [*row1,*row2,...,*rowm],[row 1 (length n)],[row 2 (length n)],...[row m (length n)]]

  Entries are accessed like normal, with mat[i][j] being the ith row, jth column.
  Note that mat[0] points directly to the block, which could be treated as a 
  vector of size m*n.
*****************************************************/

//Note: in terms of cache efficiency, the fastest matrix multiply is transposeB, and the slowest is transposeA
//The fastest matrix-vector multiply is the regular one, and the transpose multiply is slower.
//When using large matrices, this can make a huge difference!

inline matrix_type matrix_create(int m, int n)
//the layout is [m pointers][big block of m*n data]
{
	matrix_type mat;
	int i;
	if(n != 0 && m != 0)
	{
		int ofs = m*sizeof(Real*);
		mat = (Real**)malloc(m*sizeof(Real*) + m*n*sizeof(Real));
		unsigned char* bytes = (unsigned char*) mat;
		for(i=0; i<m; i++)
		{
			//mat+m = offset to block
			//i*n = offset in block
			mat[i] = (Real*)((bytes + ofs) + i*n*(sizeof(Real)));
		}
		return mat;
	}
	return NULL;
}

inline void matrix_delete(matrix_type v)
{
	if(v)
		free(v);
}


inline void matrix_print(matrix_type X, int m, int n)
{
	printf("(");
	for(int i=0; i<m; i++)
	{
		vector_print(X[i], n);
	}
	printf(")\n");
}

//X = 0
inline void matrix_zero(matrix_type X, int m, int n)
{
	
	int i;
	for(i=0; i<m; i++)
	  vector_zero(X[i], n);
}

//X = I
inline void matrix_identity(matrix_type X, int m, int n)
{
	int i,j;
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			X[i][j] = Delta(i,j);
}


//X = A
inline void matrix_equal(matrix_type X, const matrix_type A, int m, int n)
{
	int i,j;
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			X[i][j] = A[i][j];
}

//X = A
inline matrix_type matrix_copy(matrix_type A, int m, int n)
{
	matrix_type X = matrix_create(m,n);
	matrix_equal(X,A,m,n);
	return X;
}

//X = A^t.  X is mxn, A is nxm
inline void matrix_transpose(matrix_type X, const matrix_type A, int m, int n)
{
	int i,j;
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			X[i][j] = A[j][i];
}

//X = A+B
inline void matrix_add(matrix_type x, const matrix_type a, const matrix_type b, int m, int n)
{
	int i,j;
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			x[i][j] = a[i][j] + b[i][j];
}

//X = A-B
inline void matrix_sub(matrix_type x, const matrix_type a, const matrix_type b, int m, int n)
{
	int i,j;
	for(i=0; i<m; i++)
		for(j=0; j<n; j++)
			x[i][j] = a[i][j] - b[i][j];
}

//X = A*B.  X is mxp, A is mxn, B is nxp
inline void matrix_multiply(matrix_type x, const matrix_type a, const matrix_type b, int m, int n, int p)
{
	int i,j,k;
	for(i=0; i<m; i++)
	{
		for(j=0; j<p; j++)
		{
			x[i][j] = Zero;
			for(k=0; k<n; k++)
				x[i][j] += a[i][k]*b[k][j];
		}
	}
}

//X = A^t*B.  X is mxp, A is nxm, B is nxp
inline void matrix_multiply_transposeA(matrix_type x, const matrix_type a, const matrix_type b, int m, int n, int p)
{
	int i,j,k;
	for(i=0; i<m; i++)
	{
		for(j=0; j<p; j++)
		{
			x[i][j] = Zero;
			for(k=0; k<n; k++)
				x[i][j] += a[k][i]*b[k][j];
		}
	}
}

//X = A*B^t.  X is mxp, A is mxn, B is pxn
inline void matrix_multiply_transposeB(matrix_type x, const matrix_type a, const matrix_type b, int m, int n, int p)
{
	int i,j,k;
	for(i=0; i<m; i++)
	{
		for(j=0; j<p; j++)
		{
			/*
			x[i][j] = Zero;
			for(k=0; k<n; k++)
				x[i][j] += a[i][k]*b[j][k];
			*/
			x[i][j] = vector_dot(a[i],b[j],n);
		}
	}
}

//x = A*b
inline void matrix_vector_multiply(vector_type x, const matrix_type a, const vector_type b, int m, int n)
{
	int i,j;
	for(i=0; i<m; i++)
	{
		/*
		x[i] = 0;
		for(j=0; j<n; j++)
			x[i] += a[i][j]*b[j];
		*/
		x[i] = vector_dot(a[i],b,n);
	}
}

//x = A^t*b
inline void matrix_vector_multiply_transpose(vector_type x, const matrix_type a, const vector_type b, int m, int n)
{
	for(int i=0; i<n; i++)
	{
		x[i] = Zero;
		for(int j=0; j<m; j++)
			x[i] += a[j][i]*b[j];
	}
}

//returns the dot product <(row i of m),v>
inline Real matrix_row_dot(const matrix_type m, int i, const vector_type v, int n)
{
	return vector_dot(m[i], v, n);
}

//returns the dot product <(col i of m),v>
inline Real matrix_col_dot(const matrix_type m, int i, const vector_type v, int n)
{
  Real sum = Zero;
  for(int j=0; j<n; j++) {
    sum += m[j][i]*v[j];
  }
  return sum;
}

#endif

