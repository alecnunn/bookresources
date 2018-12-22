/*********************************************************************
 * svd.cpp
 * Authored by Kris Hauser 2002-2003
 *
 * Interfaces to singular value decomposition routines.
 *
 * Copyright 2003, Regents of the University of California 
 *
 *********************************************************************/

/***************************************************
 * svd.cpp
 * Kris Hauser 2002
 * Implements inversion of a matrix using singular
 * value decomposition.  SVD routine calls another
 * from Numerical Recipes in C.
 ***************************************************/

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


#include <stdio.h>
#include "fastvector.h"
#include "fastmatrix.h"
#include "misc.h"
#include <math.h>
#include <assert.h>
#include "svd.h"


#ifdef USE_CLAPACK

/*
    SGESVD computes the singular value decomposition (SVD) of a real   
    M-by-N matrix A, optionally computing the left and/or right singular   
    vectors. The SVD is written   

         A = U * SIGMA * transpose(V)   

    where SIGMA is an M-by-N matrix which is zero except for its   
    min(m,n) diagonal elements, U is an M-by-M orthogonal matrix, and   
    V is an N-by-N orthogonal matrix.  The diagonal elements of SIGMA   
    are the singular values of A; they are real and non-negative, and   
    are returned in descending order.  The first min(m,n) columns of   
    U and V are the left and right singular vectors of A.   

    Note that the routine returns V**T, not V.   

    Arguments   
    =========   

    JOBU    (input) CHARACTER*1   
            Specifies options for computing all or part of the matrix U:   
            = 'A':  all M columns of U are returned in array U:   
            = 'S':  the first min(m,n) columns of U (the left singular   
                    vectors) are returned in the array U;   
            = 'O':  the first min(m,n) columns of U (the left singular   
                    vectors) are overwritten on the array A;   
            = 'N':  no columns of U (no left singular vectors) are   
                    computed.   

    JOBVT   (input) CHARACTER*1   
            Specifies options for computing all or part of the matrix   
            V**T:   
            = 'A':  all N rows of V**T are returned in the array VT;   
            = 'S':  the first min(m,n) rows of V**T (the right singular   
                    vectors) are returned in the array VT;   
            = 'O':  the first min(m,n) rows of V**T (the right singular   
                    vectors) are overwritten on the array A;   
            = 'N':  no rows of V**T (no right singular vectors) are   
                    computed.   

            JOBVT and JOBU cannot both be 'O'.   

    M       (input) INTEGER   
            The number of rows of the input matrix A.  M >= 0.   

    N       (input) INTEGER   
            The number of columns of the input matrix A.  N >= 0.   

    A       (input/output) REAL array, dimension (LDA,N)   
            On entry, the M-by-N matrix A.   
            On exit,   
            if JOBU = 'O',  A is overwritten with the first min(m,n)   
                            columns of U (the left singular vectors,   
                            stored columnwise);   
            if JOBVT = 'O', A is overwritten with the first min(m,n)   
                            rows of V**T (the right singular vectors,   
                            stored rowwise);   
            if JOBU .ne. 'O' and JOBVT .ne. 'O', the contents of A   
                            are destroyed.   

    LDA     (input) INTEGER   
            The leading dimension of the array A.  LDA >= max(1,M).   

    S       (output) REAL array, dimension (min(M,N))   
            The singular values of A, sorted so that S(i) >= S(i+1).   

    U       (output) REAL array, dimension (LDU,UCOL)   
            (LDU,M) if JOBU = 'A' or (LDU,min(M,N)) if JOBU = 'S'.   
            If JOBU = 'A', U contains the M-by-M orthogonal matrix U;   
            if JOBU = 'S', U contains the first min(m,n) columns of U   
            (the left singular vectors, stored columnwise);   
            if JOBU = 'N' or 'O', U is not referenced.   

    LDU     (input) INTEGER   
            The leading dimension of the array U.  LDU >= 1; if   
            JOBU = 'S' or 'A', LDU >= M.   

    VT      (output) REAL array, dimension (LDVT,N)   
            If JOBVT = 'A', VT contains the N-by-N orthogonal matrix   
            V**T;   
            if JOBVT = 'S', VT contains the first min(m,n) rows of   
            V**T (the right singular vectors, stored rowwise);   
            if JOBVT = 'N' or 'O', VT is not referenced.   

    LDVT    (input) INTEGER   
            The leading dimension of the array VT.  LDVT >= 1; if   
            JOBVT = 'A', LDVT >= N; if JOBVT = 'S', LDVT >= min(M,N).   

    WORK    (workspace/output) REAL array, dimension (LWORK)   
            On exit, if INFO = 0, WORK(1) returns the optimal LWORK;   
            if INFO > 0, WORK(2:MIN(M,N)) contains the unconverged   
            superdiagonal elements of an upper bidiagonal matrix B   
            whose diagonal is in S (not necessarily sorted). B   
            satisfies A = U * B * VT, so it has the same singular values   
            as A, and singular vectors related by U and VT.   

    LWORK   (input) INTEGER   
            The dimension of the array WORK. LWORK >= 1.   
            LWORK >= MAX(3*MIN(M,N)+MAX(M,N),5*MIN(M,N)).   
            For good performance, LWORK should generally be larger.   

            If LWORK = -1, then a workspace query is assumed; the routine   
            only calculates the optimal size of the WORK array, returns   
            this value as the first entry of the WORK array, and no error   
            message related to LWORK is issued by XERBLA.   

    INFO    (output) INTEGER   
            = 0:  successful exit.   
            < 0:  if INFO = -i, the i-th argument had an illegal value.   
            > 0:  if SBDSQR did not converge, INFO specifies how many   
                  superdiagonals of an intermediate bidiagonal form B   
                  did not converge to zero. See the description of WORK   
                  above for details. 

int sgesvd_(char *jobu, char *jobvt, integer *m, integer *n, 
	real *a, integer *lda, real *s, real *u, integer *ldu, real *vt, 
	integer *ldvt, real *work, integer *lwork, integer *info)
*/

extern "C" {
  #include <f2c.h>
  //#include <clapack.h>

  int sgesvd_(char *jobu, char *jobvt, integer *m, integer *n, 
	     real *a, integer *lda, real *s, real *u, integer *ldu, real *vt, 
	     integer *ldvt, real *work, integer *lwork, integer *info);

  int sgesvd2_(char *jobu, char *jobvt, integer *m, integer *n, 
	     real *a, integer *lda, real *s, real *u, integer *ldu, real *vt, 
	     integer *ldvt, real *work, integer *lwork, integer *info);

};

void matrix_block_print(const Real* A, int m, int n)
{
  for(int i=0; i<m; i++) {
    for(int j=0; j<n; j++)
      printf("%f ", A[i+j*n]);
    printf("\n");
  }
}

void svdcmp_CLAPACK(Real* A, integer m, integer n, Real* U, Real* w, Real* Vt)
{
  char jobu;
  char jobvt;
  
  integer work_size;
  real temp;
  real* workspace;
  integer ldu = m; 
  integer res;
  integer info = 0;

  printf("CLAPACK SVD\n");
  assert(m > 0 && n > 0);

  //query an optimal workspace size
  jobu = 'A';
  jobvt = 'A';
  temp = 0;
  work_size = -1;
  res = sgesvd2_(&jobu, &jobvt, &m, &n,
	  NULL, &m, NULL, NULL, &ldu, NULL,
	  &n, &temp, &work_size, &info);
  if(res != 0) {
    printf("Error querying SVD workspace size\n");
    abort();
  }
  work_size = (integer)temp;
  printf("work size is %d\n", work_size);
  workspace = (real*) malloc(work_size*sizeof(real));

  assert(m > 0 && n > 0);

  /*printf("m=%d, n=%d\n", m, n);
  matrix_block_print(A, m, n);
  printf("\n");
  matrix_block_print(U, m, m);
  printf("\n");
  matrix_block_print(Vt, n, n);
  printf("\n");*/
  res = sgesvd2_(&jobu, &jobvt, &m, &n,
	  A, &m, w, U, &ldu, Vt,
	  &n, workspace, &work_size, &info);
  //abort();

  free(workspace);

  if(res != 0) {
    printf("SVD Unsuccessful, failure %d, info %d\n", res, info);
    abort();
  }
}

void svdcmp_CLAPACK(matrix_type A, int m, int n, vector_type w, matrix_type Vt)
{
  matrix_type A_col = matrix_create(n,m);
  matrix_transpose(A_col, A, m, n);
  matrix_type U_col = matrix_create(m,m);
  svdcmp_CLAPACK(&A_col[0][0], m, n, &U_col[0][0], w, &Vt[0][0]);
  matrix_transpose(A, U_col, m, m);
  for(int i=0; i<m; i++)
    for(int j=m; j<n; j++)
      A[i][j] = 0;
  matrix_delete(A_col);
  matrix_delete(U_col);
}


#endif  //USE_CLAPACK




inline vector_type convert_vector_to_1_based(vector_type x)
{
	return x-1;
}

inline vector_type convert_vector_to_0_based(vector_type x)
{
	return x+1;
}

inline matrix_type convert_matrix_to_1_based(matrix_type x, int m, int n)
{
	for(int i=0; i<m; i++)
		x[i] = convert_vector_to_1_based(x[i]);
	return x-1;
}

inline matrix_type convert_matrix_to_0_based(matrix_type x, int m, int n)
{
	for(int i=1; i<=m; i++)
		x[i] = convert_vector_to_0_based(x[i]);
	return x+1;
}





/*
Given a matrix a[1..m][1..n], this routine computes its singular value decomposition, A =
U.W.Vt. The matrix U replaces a on output. The diagonal matrix of singular values W is output
as a vector w[1..n]. The matrix V (not the transpose V T ) is output as v[1..n][1..n].
*/

void svdcmp_NR(matrix_type a, int m, int n, vector_type w, matrix_type v)
{
	a = convert_matrix_to_1_based(a,m,n);
	w = convert_vector_to_1_based(w);
	v = convert_matrix_to_1_based(v,n,n);

	svdcmp(a,m,n,w,v);

	a = convert_matrix_to_0_based(a,m,n);
	v = convert_matrix_to_0_based(v,n,n);
	w = convert_vector_to_0_based(w);
}






void test_svd_solution(matrix_type a, int m, int n, matrix_type u, vector_type w, matrix_type v) {
  matrix_type temp = matrix_create(m,n);
  matrix_type uw = matrix_create(m,n);

  for(int i=0; i<m; i++) {
    for(int j=0; j<m; j++)
      uw[i][j] = u[i][j]*w[j];
  }
  for(int i=0; i<m; i++) {
    for(int j=m; j<n; j++)
      uw[i][j] = 0.0;
  }

  matrix_multiply_transposeB(temp, uw, v, m,n,n);

  //printf("Original A:\n");
  //matrix_print(a, m, n);
  //printf("SVD's A:\n");
  //matrix_print(temp, m, n);

  matrix_sub(temp, a, temp, m, n);
  printf("Difference:\n");
  matrix_print(temp, m, n);

  matrix_delete(uw);
  matrix_delete(temp);
}

Real frand_scale = 1.0/(Real)RAND_MAX;

Real frand(Real min, Real max)
{
  Real u = (Real)rand()*frand_scale;
  return min + u*(max-min);
}

void matrix_rand(matrix_type A, int m, int n, Real min, Real max)
{
  int i,j;
  for(i=0; i<m; i++)
    for(j=0; j<n; j++)
      A[i][j] = frand(min, max);
}

void svd_test(void (*svd) (matrix_type, int, int, vector_type, matrix_type), int m, int n)
{
  matrix_type a = matrix_create(m,n);
  vector_type w = vector_create(n);
  matrix_type v = matrix_create(n,n);

  if(m > n)
    abort();

  matrix_rand(a, m, n, 0, 10);

  matrix_type acopy = matrix_copy(a, m, n);

  svd(a, m,n, w, v);

  test_svd_solution(acopy, m, n, a, w, v);
    
  matrix_delete(a);
  vector_delete(w);
  matrix_delete(v);
  matrix_delete(acopy);
}

void svdecomp(matrix_type a, int m, int n, vector_type w, matrix_type v)
{
  if(m == 0 || n == 0)
    return;

  //printf("Matrix to svdecomp\n");
  //matrix_print(a, m, n);

  //printf("CLAPACK:\n");
  //svd_test(svdcmp_CLAPACK, m, n);
  //printf("\nNR:\n");
  //svd_test(svdcmp_NR, m, n);
  //exit(0);

  //matrix_type acopy = matrix_copy(a, m, n);
  //svdcmp_CLAPACK(a,m,n,w,v);
  svdcmp_NR(a,m,n,w,v);

  //test_svd_solution(acopy, m,n, a,w,v);
  //matrix_delete(acopy);

  //abort();
}




#define g_EPSILON 0.00001

//x is size n, b is size m
//solves Ax = b where A = U.W.Vt are the matrices output by svdcmp
void svdbksub(const matrix_type u, const vector_type w, const matrix_type vt, int m, int n, vector_type x, const vector_type b)
{
	int j,i;
	Real s;

	if(n == 0)
	  return;

	vector_type tmp=vector_create(n);
	for (j=0;j<n;j++) { //Calculate Ut*b.
		s=0.0;
		if (w[j] > g_EPSILON) { //Nonzero result only if wj is nonzero.
			for (i=0;i<m;i++) s += u[i][j]*b[i];
			s /= w[j]; //This is the divide by wj .
		}
		tmp[j]=s;
	}
	matrix_vector_multiply(x, vt, tmp, n,n);
	vector_delete(tmp);
}



//solves for the pseudo inverse of A, where A is size m by n
void svd_minv_solve(matrix_type A, matrix_type Ainv, int m, int n)
{
	int i,j;

	if(m == 0 || n == 0)
	  return;

//	printf("A:\n");
//	matrix_print(A,m,n);
//	matrix_type Acopy = matrix_copy(A,m,n);

	vector_type w = vector_create(n);
	matrix_type V = matrix_create(n,n);
	svdecomp(A,m,n, w,V);

	vector_type col = vector_create(n);
	for(i=0; i<n; i++)
	{
		for(j=0; j<n; j++)
			col[j] = (i==j? 1.0 : 0.0);
		svdbksub(A,w,V,m,n,col,col);
		for(j=0; j<n; j++)
			Ainv[j][i] = col[j];
	}
	vector_delete(col);

	//w = convert_vector_to_0_based(w);
	//V = convert_matrix_to_0_based(V,n,n);
	vector_delete(w);
	matrix_delete(V);

/*	printf("A inv:\n");
	matrix_print(Ainv,n,m);
	matrix_type prod = matrix_create(m,m);
	matrix_multiply(prod, Acopy, Ainv, m,n,m);
	printf("Product:\n");
	matrix_print(prod, m, m);
	matrix_delete(prod);
	matrix_delete(Acopy);*/
}


void svd_ls_solve(matrix_type A, int m, int n, vector_type x, const vector_type b)
{
	vector_type w = vector_create(n);
	matrix_type V = matrix_create(n,n);
	svdecomp(A,m,n, w,V);

	svdbksub(A,w,V,m,n,x,b);

	vector_delete(w);
	matrix_delete(V);

}

void svd_ls_solve(matrix_type A, int m, int n, vector_type* x, const vector_type* b, int k)
{
	int i;

	vector_type w = vector_create(n);
	matrix_type V = matrix_create(n,n);
	svdecomp(A,m,n, w,V);

	for(i=0; i<k; i++)
	  svdbksub(A,w,V,m,n,x[i],b[i]);

	vector_delete(w);
	matrix_delete(V);

}

