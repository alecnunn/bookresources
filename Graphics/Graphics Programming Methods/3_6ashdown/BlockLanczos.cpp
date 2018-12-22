////////////////////////////////////////////////////////////////////////
//
//  BLOCKLANCZOS.CPP - Iterative Block Lanczos Eigensolver Class Module
//
//  Version:    4.00A
//
//  History:    00/05/30 - Created.
//				00/12/18 - Modified Init to allocate (eva + 1) rather
//						   than (order + 1) elements for pval.
//				01/02/01 - Modified user-defined function prototype
//						   puser in Init.
//						 - Added large to Init parameter list.
//						 - Removed small from Init parameter list.
//						 - Modified puser function calls in
//						   PerformMethod and CalcDiagonal.
//				01/04/01 - Modified Init to ensure that eva_size is at
//						   least as large as init_size.
//				03/03/23 - Revised copyright notice.
//
//  Compilers:  Microsoft Visual C/C++ Professional V7.0
//
//  Reference:	Underwood, R. R. 1975. An Iterative Block Lanczos Method
//				for the Solution of Large Sparse Symmetric
//				Eigenproblems, Technical Report STAN-CS-75-495,
//			    Department of Computer Science, Stanford University,
//				Stanford, CA.
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 2003 byHeart Consultants Limited
//
//  This software may be freely copied, redistributed, and/or modified
//  as long as the copyright notice is included with all source code
//  files.
//
////////////////////////////////////////////////////////////////////////

// INCLUDE FILES
#include "BlockLanczos.h"

////////////////////////////////////////////////////////////////////////
//
//  Init - Initialize Eigensystem Calculations
//
//  Purpose:	To initialize the iterative block Lanczos algorithm
//				parameters.
//
//  Call:		bool Init
//				(
//					bool large,
//					int n,
//					int r,
//					int q,
//					int ibs,
//					int mmax,
//					void (*pop)( double *pu, double *pv, int num, bool
//					        inv ),
//					double eps
//				)
//
//  Where:		large is a Boolean flag which is true indicates that the
//				  largest eigenvalues are to be computed; otherwise the
//				  smallest eigenvalues are computed.
//				n is the order of the symmetric matrix. It must be
//				  greater than 1.
//				r is the maximum number of eigenvalues and corresponding
//				  eigenvectors to be computed. It must be greater than 0
//				  and less than n.
//				q is the number of vectors in the eigenvector array to
//				  be allocated. It must be greater than r and less than
//				  or equal to n.
//				ibs is the initial block size to be used by the block
//				  Lanczos algorithm.
//				mmax is the maximum allowable number of calls to the
//				  user-specified function pointed to by puser.
//				pop is a pointer to a user-defined function that
//				  defines the sparse matrix A. This function must accept
//				  four arguments:
//
//				    pu	- a pointer to an input vector u.
//					pv	- a pointer to where the vector v is to be
//						    returned.
//					num - the order of the matrix.
//					inv - a Boolean flag which is true indicates that
//                          the negative of the matrix is to be used.
//
//				  and calculate v = A * u, where u can be overwritten by
//				  the function if necessary.
//				eps is the relative precision to which Calculate will
//				  attempt to compute the eigenvalues and eigenvectors.
//				  For eigenvalues less in modulus than 1.0, eps will be
//				  an absolute tolerance. (Because of the way the
//				  iterative block Lanczos algorithm works, it may happen
//				  that the later eigenvalues cannot be computed to the
//				  same relative precision as those less in value.)
//
//  Return:		true if successful; otherwise false (out of memory).
//
////////////////////////////////////////////////////////////////////////

bool BlockLanczos::Init( bool large, int n, int r, int q, int ibs,
		int mmax, void (*pop) ( double *, double *, int, bool ),
		double eps	)
{
	bool status = true;		// Return status
	int i;					// Loop index

	// Initialize parameters
	neg_flag = large;
	order = n;
	max_eig = r;
	eva_size = __max(q, ibs);
	init_size = ibs;
	max_iter = mmax;
	puser = pop;
	prec = eps;

	// Validate input parameters
	if (order > 1 || max_eig > 0 || eva_size > max_eig || eva_size
			<= order)
	{
		num_calc = 0;	// Number of calculated eigenvalues

		if (status == true)
		{
			// Allocate eigenvalue array
			if ((pval = new double [eva_size + 1]) == NULL)
				status = false;
		}

		// Allocate eigenvector array
		if ((ppvec = new double *[eva_size + 1]) == NULL)
			status = false;

		if (status == true)
		{
			for (i = 1; i <= eva_size; i++)
			{
				if ((ppvec[i] = new double [order + 1]) == NULL)
				{
					status = false;
					break;
				}
			}
		}

		if (status == true)
		{
			// Allocate array diagonal elements vector
			if ((pdiag = new double [eva_size + 1]) == NULL)
				status = false;
		}

		if (status == true)
		{
			// Allocate Euclidean length array
			if ((peuc = new double [eva_size + 1]) == NULL)
				status = false;
		}

		if (status == true)
		{
			// Allocate input vector
			if ((pu = new double [order + 1]) == NULL)
				status = false;
		}

		if (status == true)
		{
			// Allocate output vector
			if ((pv = new double [order + 1]) == NULL)
				status = false;
		}

		if (status == true)
		{
			// Allocate matrix
			if ((pmat = new double [(eva_size + 1) * (eva_size + 1)])
					== NULL)
				status = false;
		}

		if (status == true)
		{
			// Randomize initial eigenvectors
			for (i = 1; i <= init_size; i++)
				Randomize(i);
		}
		else
			Release();	// Release memory
	}
	else
		status = false;

	return status;
}

////////////////////////////////////////////////////////////////////////
//
//  Calculate - Calculate Eigenvalues and Corresponding Eigenvectors
//
//  Purpose:	To calculate the eigenvalues and corresponding
//				eigenvectors of a symmetric matrix A using the iterative
//				block Lanczos method.
//
//  Call:		bool Calculate
//				(
//					int *pnc
//				)
//
//  Where:		pnc is a pointer to where the number of converged
//				  eigenvalues is to be returned.
//
//  Return:		true if successful; otherwise false (termination due to
//				unrecoverable failure to converge).
//
//  Note:		This function may be iterated.
//
////////////////////////////////////////////////////////////////////////

bool BlockLanczos::Calculate( int *pnc )
{
	int ps;

	// Initialize current block size
	curr_size = init_size;

	// Determine the number of times that the block Lanczos algorithm is
	// to be iterated
	num_steps = (eva_size - num_calc) / curr_size;

	if (num_steps < 2)
	{
		num_steps = 2;
		curr_size = eva_size / 2;
	}

	if (num_calc == 0)
	{
		
		Reorthogonalize(num_calc);	// Reorthogonalize Lanczos vectors

		if (CalcDiagonal() == false)	// Calculate diagonal matrix
		{
			*pnc = num_calc;

			return false;
		}

		acc_err = 0.0;
	}

	num_iter = 0;	// Initialize iteration count
	num_mvm = 0;	// Initialize matrix-vector multiplications count

	while (num_calc < max_eig)
	{
		// Check for maximum number of matrix-vector multiplications
		if (num_mvm > max_iter)
		{
			init_size = curr_size;	// Reinitialize initial block size
			break;
		}

		num_iter++;
		ps = curr_size * num_steps;

		// Carry out the block Lanczos method and store the resulting
		// block tridiagonal matrix MS in pmat and the order-by-ps
		// orthonormal matrix XS in ppvec. The initial order-by-
		// curr_size orthonormal matrix is assumed to be stored in
		// columns (num_calc + 1) through (num_calc + px) of ppvec.
		// The residuals for these vectors and the eigenvalue
		// approximations in pval are computed and stored in peuc.
		//
		PerformMethod();

		// Solve the eigenproblem for MS, storing the eigenvalues
		// in elements (num_calc + 1) through (num_calc + ps) of
		// pval and the eigenvectors in the first curr_size *
		// num_steps rows and columns of pmat (possibly overwriting
		// MS).
		//
		CalcEigen(curr_size, ps);

		// Determine how many of the eigenvalues and eigenvectors have
		// converged using the error estimates stored in peuc. The
		// number that have converged is stored in num_conv.
		//
		TestConvergence();

		// Choose new values for the current block size and the number
		// of steps for the block Lanczos method.
		//
		ChooseBlockSize();	

		// Compute the eigenvectors of the restricted matrix using XS
		// stored in ppvec and the eigenvectors of MS stored in pmat.
		// These vectors serve both as eigenvector approximations and
		// to form the matrix used to start the block lanczos method in
		// the next iteration.
		//
		Rotate(ps, num_conv + curr_size);

		num_calc += num_conv;
		num_mvm += curr_size * num_steps;
	}

	*pnc = num_calc;

	return true;
}

////////////////////////////////////////////////////////////////////////
//
//  PerformMethod - Perform Block Lanczos Method
//
//  Purpose:	To perform the block Lanczos method..
//
//  Call:		void PerformMethod()
//
//  Note:		This function implements the block Lanczos method with
//				reorthogonalization. It computes a block tridiagonal
//              matrix MS which it stores in rows and columns
//				(num_calc + 1) through (num_calc + curr_size *
//				num_steps) of the array pmat and an orthonormal matrix
//				XS which it stores in columns (num_calc + 1) through
//				(num_calc + curr_size * num_steps) of the array
//				ppvec[order][eva_size]. MS is a symmetric matrix with
//				curr_size-by-curr_size symmetric matrices M(1) ,..
//				M(num_steps) on its diagonal and curr_size-by-curr_size
//				upper triangular matrices R(2),...,R(num_steps) along
//				its lower diagonal. Because MS is symmetric and banded,
//				only its lower triangle (curr_size + 1 diagonals) is
//				stored in pmat. XS is composed of num_step order-by-
//				curr_size orthonormal matrices ppvec[1] ...
//				ppvec(num_steps), where ppvec[1] is given and should be
//				stored in columns (num_calc + 1) through
//				(num_calc + curr_size) of ppvec.
//
//				Furthermore, ppvec[1] is assumed to satisfy ppvec[1] *
//				A * ppvec[1] = diag(pval[num_calc + 1],
//				ppval[num_calc + 2]. ... ppval[num_calc + curr_size],
//				and if num_calc > 0, then ppvec[1] is assumed to be
//				orthogonal to the vectors stored in columns 1 through
//				num_calc of ppvec.
//
//				During the first step, Err is called and the quantities
//				Ej are computed, where Ej = ||A * X1j -
//				pval[num_calc + j)] * X1j||, X1j is the jth column of
//				ppvec[1], and ||*|| denotes the Euclidean norm. Ej is
//				stored in E[num_calc + j], j = 1, ..., curr_size.
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::PerformMethod()
{
	double t;
	int il;
	int it;
	int i, j, k, l;	// Loop indices
	int kp1;
	int ll;
	int lu;
	int mp1;
	int mpps;

	mp1 = num_calc + 1;
	mpps = num_calc + curr_size * num_steps;

	for (l = 1; l <= num_steps; l++)
	{
		ll = num_calc + (l - 1) * curr_size + 1;
		lu = num_calc + l * curr_size;

		for (k = ll; k <= lu; k++)
		{
			for (i = 1; i <= order; i++)
				pu[i] = ppvec[k][i];

			// Perform user-defined matrix-vector multiplication
			puser(pu, pv, order, neg_flag);

			if (l <= 1)
			{
				for (i = k; i <= lu; i++)
					pmat[k * eva_size + i] = 0.0;

				pmat[k * eva_size + k] = pval[k];

				for (i = 1; i <= order; i++)
					pv[i] -= pval[k] * ppvec[k][i];
			}
			else
			{
				for (i = k; i <= lu; i++)
				{
					t = 0.0;

					for (j = 1; j <= order; j++)
						t += pv[j] * ppvec[i][j];

					pmat[k * eva_size + i] = t;
				}

				it = k - curr_size;

				for (i = 1; i <= order; i++)
				{
					t = 0.0;

					for (j = it; j <= k; j++)
						t += ppvec[j][i] * pmat[j * eva_size + k];

					if (k != lu)
					{
						kp1 = k + 1;

						for (j = kp1; j <= lu; j++)
							t += ppvec[j][i] * pmat[k * eva_size + j];
					}

					pv[i] -= t;
				}
			}

			if (l != num_steps)
			{
				for (i = 1; i <= order; i++)
					ppvec[k + curr_size][i] = pv[i];
			}
		}

		if (l == 1)
			CalcNorm();

		if (l == num_steps)
			continue;

		Reorthogonalize(lu);

		il = lu + 1;
		it = lu;

		for (j = 1; j <= curr_size; j++)
		{
			it++;

			for (i = il; i <= it; i++)
				pmat[(it - curr_size) * eva_size + i] =
						pmat[it * eva_size + i];
		}
	}
}

////////////////////////////////////////////////////////////////////////
//
//  CalcNorm - Compute Euclidean Norm
//
//  Purpose:	To compute the Euclidean lengths of vectors..
//
//  Call:		void CalcNorm()
//
//  Note:		This function computes the Euclidean lengths of the 
//				vectors stored in the columns (num)calc + curr_size + 1)
//				through (num_calc + curr_size + curr_size) of array
//				ppvec[order][eva_size] and stores them in elements
//				(num_calc + 1) through (num_calc + curr_size) of peuc.
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::CalcNorm()
{
	double t;		// Temporary value
	int i, k;		// Loop indices
	int mp1, mpp;	// Temporary values

	mp1 = num_calc + curr_size + 1;
	mpp = num_calc + curr_size + curr_size;

	for (k = mp1; k <= mpp; k++)
	{
		t = 0.0;

		for (i = 1; i <= order; i++)
			t += ppvec[k][i] * ppvec[k][i];

		peuc[k - curr_size] = sqrt(t);
	}
}

////////////////////////////////////////////////////////////////////////
//
//  Reorthogonalize - Reorthogonalize Lanczos Vectors
//
//  Purpose:	To reorthogonalize a Lanczos vector array.
//
//  Call:		void Reorthogonalize
//				(
//					int f
//				)
//
//  Where:      f is a column offset.
//
//  Note:       This function reorthogonalizes the matrix
//              Z[curr_size][order] stored in columns (f + 1) through
//              (f + curr_size) of ppvec with respect to the vectors
//              stored in the first f columns of ppvec and then
//              decomposes the resulting matrix into the product of the
//              orthonormal matrix XORTH[curr_size][order], say, and an
//              upper triangular matrix R[curr_size][curr_size].
//
//              XORTH is stored over Z and the upper triangle of R is
//              stored in rows and columns (f + 1) through
//              (f + curr_size) of pmat.
//
//				A stable variant of the Gram-Schmidt orthogonalization
//				method is utilized.
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::Reorthogonalize( int f )
{
	bool orig;
	double s, t;
	int fp1, fpp;
	int i, j, k;	// Loop indices
	int km1;

	if (curr_size == 0)
		return;

	fp1 = f + 1;
	fpp = f + curr_size;

	for (k = fp1; k <= fpp; k++)
	{
		orig = true;

		km1 = k - 1;

		for ( ; ; )
		{
			t = 0.0;

			if (km1 > 0)
			{
				for (i = 1; i <= km1; i++)
				{
					s = 0.0;

					for (j = 1; j <= order; j++)
						s += ppvec[i][j] * ppvec[k][j];

					if (orig == true && i > f)
						pmat[k * eva_size + i] = s;

					t += s * s;

					for (j = 1; j <= order; j++)
						ppvec[k][j] -= s * ppvec[i][j];
				}
			}

			s = 0.0;

			for (j = 1; j <= order; j++)
				s += ppvec[k][j] * ppvec[k][j];

			t += s;

			if (s > t / 100.0)
				break;
			else
				orig = false;
		}

		s = sqrt(s);

		pmat[k * eva_size + k] = s;

		if (s != 0.0)
			s = 1.0 / s;

		for (j = 1; j <= order; j++)
			ppvec[k][j] *= s;
	}
}

////////////////////////////////////////////////////////////////////////
//
//  CalcDiagonal - Calculate Diagonal Matrix 
//
//  Purpose:	To calculate the diagonal matrix CP = X1' * A * X1 and
//				its eigenvalues and eigenvectors.
//
//  Call:		bool CalcDiagonal()
//
//  Return:		true if successful; otherwise false (failure to
//				converge).
//
//  Note:		This function transforms the orthonormal matrix
//				X1[curr_size][order], say, stored in columns
//				(num_calc + 1) through (num_calc + curr_size) of ppvec
//				so that X1' * A * X1 = diag(d1, ..., dp) = CP, where '
//				denotes transpose and A is the sparse symmetric matrix
//				defined by the function *puser. The values d1, ..., dp
//				are stored in elements (num_calc + 1) through (num_calc
//				+ curr_size) of pval.
//
////////////////////////////////////////////////////////////////////////

bool BlockLanczos::CalcDiagonal()
{
	double t;			// Temporary value
	int i, j, k;		// Loop indices
	int icol1, icol2;	// Array indices

	icol1 = num_calc;

	for (j = 1; j <= curr_size; j++)
	{
		icol1++;

		for (i = 1; i <= order; i++)
			pu[i] = ppvec[icol1][i];

		// Perform user-defined matrix-vector multiplication
		puser(pu, pv, order, neg_flag);

		icol2 = num_calc;

		for (i = 1; i <= j; i++)
		{
			icol2++;
			t = 0.0;

			for (k = 1; k <= order; k++)
				t += pv[k] * ppvec[icol2][k];

			pmat[icol2 * eva_size + icol1] = t;
		}
	}

	// Calculate eigenvalues and eigenvectors QP of CP
	if (CalcEigen(curr_size, curr_size) == true)
	{
		Rotate(curr_size, curr_size);	// Calculate X1 * QP

		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////
//
//  TestConvergence - Test For Convergence
//
//  Purpose:	To determine which eigenvalues have converged.
//
//  Call:		void TestConvergence()
//
//  Note:		This function determines which of the curr_size
//				eigenvalues stored in elements (num_calc + 1) through
//				(num_calc + curr_size) of pval have converged.
//
//				acc_err is a measure of the accumulated error in the
//				num_calc previously computed eigenvalues and
//				eigenvectors. It is updated if more approximations have
//				converged.
//
//				The norms of the residual vectors are stored in elements
//				(num_calc + 1) through (num_calc + curr_size) of peuc.
//
//				prec is the precision to which we are computing the
//				approximations.
//
//				num_conv is the number that have converged. If
//				num_conv = 0, then none have converged
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::TestConvergence()
{
	double t;	// Temporary value
	int i;		// Loop index
	int k;

	k = 0;

	for (i = 1; i <= curr_size; i++)
	{
		t = __max(fabs(pval[num_calc + i]), 1.0);

		if (peuc[num_calc + i] > (t * (prec + 2.22e-15 * order) +
				acc_err))
			break;

		k = i;
	}

	num_conv = k;

	if (k == 0)
		return;

	t = 0.0;

	for (i = 1; i <= k; i++)
		t += peuc[num_calc + i] * peuc[num_calc + i];

	acc_err = sqrt(acc_err * acc_err + t);
}

////////////////////////////////////////////////////////////////////////
//
//  ChooseBlockSize - Choose Block Size
//
//  Purpose:	To choose new values or the current block size and
//				number of steps for the block Lanczos method.
//
//  Call:		void ChooseBlockSize()
//
//  Note:		This function chooses new values for curr_size and
//				num_steps. The strategy used here is to choose curr_size
//				to be the smaller of the two following values:
//
//				1) the previous block size and;
//				2) the number of values left to be computed.
//
//				num_steps is chosen as large as possible subject to the
//				constraints imposed by the limits of storage. In any
//				event, num_steps is greater than or equal to 2.
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::ChooseBlockSize()
{
	int mt;
	int pt;
	int st;

	mt = num_calc + num_conv;
	pt = max_eig - mt;

	if (pt > curr_size)
		pt = curr_size;

	if (pt == 0)
	{
		curr_size = 0;
		return;
	}

	st = (eva_size - mt) / pt;

	if (st <= 2)
	{
		st = 2;
		pt = (eva_size - mt) / 2;
	}

	curr_size = pt;
	num_steps = st;
}

////////////////////////////////////////////////////////////////////////
//
//  CalcEigen - Calculate Eigenvalues and Eigenvectors
//
//  Purpose:	To calculate the eigenvalues and eigenvectors of a
//				symmetric matrix using tridiagonalization and
//              Householder transformations.
//
//  Call:		bool CalcEigen
//				(
//					int p,
//					int ps
//				)
//
//  Where:      p is the current block size.
//              ps is the matrix order.
//
//	Return:		true if successful; otherwise false (failure to
//				converge).
//
//  Note:		This function solves the eigenproblem for the symmetric
//				matrix MS of order ps stored in rows and columns
//				(num_calc + 1) through (num_calc + ps) of pmat. The
//				eigenvalues of MS are stored in elements (num_calc + 1)
//				through (num_calc + ps) of pval and the eigenvactors are
//				stored in rows and columns 1 through ps of pmat,
//				possibly overwriting MS.
//
//				This function simply re-stores MS in a manner acceptable
//				to tred2 and tql2.
//
////////////////////////////////////////////////////////////////////////

bool BlockLanczos::CalcEigen( int p, int ps)
{
	int i, j;		// Loop indices
	int lim;
	int lm1;

	for (i = 1; i <= ps; i++)
	{
		lim = i - p;

		if (i <= p)
			lim = 1;

		if (lim > 1)
		{
			lm1 = lim - 1;

			for (j = 1; j <= lm1; j++)
				pmat[j * eva_size + i] = 0.0;
		}

		for (j = lim; j <= i; j++)
			pmat[j * eva_size + i] = pmat[(j + num_calc) * eva_size +
					(i + num_calc)];
	}

	// Tridiagonalize matrix
	Tred2(eva_size, ps, pmat, pdiag, pv, pmat);

	// Calculate eigenvalues and eigenvectors
	if (Tql2(eva_size, ps, pdiag, pv, pmat) == 0)
	{
		// Save eigenvalues
		for (i = 1; i <= ps; i++)
			pval[num_calc + i] = pdiag[i];

		return true;
	}
	else
		return false;
}

////////////////////////////////////////////////////////////////////////
//
//  Rotate -  Rotate Matrix
//
//  Purpose:	To rotate a matrix
//
//  Call:		void Rotate
//				(
//					int ps,
//					int l
//				)
//
//  Where:      ps is the order of the matrix QS.
//				l is the number of eigenvectors to be computed.        
//
//  Note:		This function computes the first l columns of the
//				matrix XS * QS, where XS is an order-by-ps orthonormal
//				matrix stored in columns (num_calc + 1) through
//				(num_calc + ps) of ppvec and QS is a ps-by-ps
//				orthonormal matrix stored in rows and columns 1 through
//				ps of pmat. The result is stored in columns
//				(num_calc + 1) through (num_calc + l) of ppvec,
//				overwriting part of XS.
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::Rotate( int ps, int l )
{
	double t;		// Temporary value
	int i, j, k;	// Loop indices

	for (i = 1; i <= order; i++)
	{
		for (k = 1; k <= l; k++)
		{
			t = 0.0;

			for (j = 1; j <= ps; j++)
				t += ppvec[num_calc + j][i] * pmat[k * eva_size + j];

			pv[k] = t;
		}

		for (k = 1; k <= l; k++)
			ppvec[num_calc + k][i] = pv[k];
	}
}

////////////////////////////////////////////////////////////////////////
//
//  Tred2 - Tridiagonalize Symmetric Matrix
//
//  Purpose:	To reduce a real symmetric matrix to a symmetric
//				tridiagonal matrix using and accumulating orthogonal
//				similarity transformations.
//
//  Call:		void Tred2
//				(
//					int nm,
//					int n.
//					double *pa,
//					double *pd,
//					double *pe,
//					double *pz
//				)
//
//  Where:      nm is the row dimension of the two-dimensional
//                array parameters.
//				n is the matrix order.
//				pa is a pointer to the matrix.
//				pd is a pointer to a vector where the diagonal elements
//				  of the tridiagonal matrix are to be returned.
//				pe is a pointer to a vector where the subdiagonal
//				  elements of the tridiagonal matrix in its last n-1
//				  positions are to be returned. pe[1] is set to zero.
//				pz is a pointer to a vector where the orthogonal
//				  transformation matrix produced in the reduction are to
//				  be returned.
//
//  Note:		This function is a C++ translation of the EISPACK
//				function TRED2.
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::Tred2( int nm, int n, double *pa, double *pd,
		double *pe, double *pz )
{
	double f, g, h, hh, scale;
	int i, j, k, l, ii, jp1;

	for (i = 1; i <= n; i++)
	{
		for (j = 1; j <= n; j++)
			pz[i * nm + j] = pa[i * nm + j];

		pd[i] = pa[i * nm + n];
	}

	if (n == 1)
		goto _510;

	// for i = n step -1 until 2 do
	for (ii = 2; ii <= n; ii++)
	{
		i = n + 2 - ii;
		l = i - 1;
		h = 0.0;
		scale = 0.0;

		if (l < 2)
			goto _130;

		// Scale row (Algol tol then not needed)
		for (k = 1; k <= l; k++)
			scale += fabs(pd[k]);

		if (scale != 0.0)
			goto _140;
_130:
		pe[i] = pd[l];

		for (j = 1; j <= l; j++)
		{
            pd[j] = pz[j * nm + l];
            pz[j * nm + i] = 0.0;
            pz[i * nm + j] = 0.0;
		}

		goto _290;
_140:
		for (k = 1; k <= l; k++)
		{
			pd[k] /= scale;
			h += pd[k] * pd[k];
		}

        f = pd[l];

		g = -dsign(sqrt(h),f);

        pe[i] = scale * g;
        h -= f * g;
        pd[l] = f - g;

		// Form a * u
		for (j = 1; j <= l; j++)
			pe[j] = 0.0;

		for (j = 1; j <= l; j++)
		{
			f = pd[j];
			pz[i * nm + j] = f;
			g = pe[j] + pz[j * nm + j] * f;

			jp1 = j + 1;

			if (l >= jp1)
			{
				for (k = jp1; k <= l; k++)
				{
					g += pz[j * nm + k] * pd[k];
					pe[k] += pz[j * nm + k] * f;
				}
			}

			pe[j] = g;
		}

		// Form p
		f = 0.0;

		for (j = 1; j <= l; j++)
		{
			pe[j] /= h;
			f += pe[j] * pd[j];
		}

		hh = f / (h + h);

		// Form q
		for (j = 1; j <= l; j++)
			pe[j] -= hh * pd[j];
		
		// Form reduced a
		for (j = 1; j <= l; j++)
		{
			f = pd[j];
            g = pe[j];

			for (k = j; k <= l; k++)
				pz[j * nm + k] -= (f * pe[k] + g * pd[k]);

            pd[j] = pz[j * nm + l];
            pz[j * nm + i]= 0.0;
		}
_290:
		pd[i] = h;
	}

	// Accumulation of transformation matrices
	for (i = 2; i <= n; i++)
	{
		l = i - 1;
		pz[l * nm + n] = pz[l * nm + l];
		pz[l * nm + l] = 1.0;
		h = pd[i];

		if (h != 0.0)
		{
			for (k = 1; k <= l; k++)
				pd[k] = pz[i * nm + k] / h;

			for (j = 1; j <= l; j++)
			{
				g = 0.0;

				for (k = 1; k <= l; k++)
					g += pz[i * nm + k] * pz[j * nm + k];

				for (k = 1; k <= l; k++)
					pz[j * nm + k] -= g * pd[k];
			}
		}

		for (k = 1; k <= l; k++)
			pz[i * nm + k] = 0.0;
	}

_510:
	for (i = 1; i <= n; i++)
	{
		pd[i] = pz[i * nm + n];
		pz[i * nm + n] = 0.0;
	}

	pz[n * nm + n] = 1.0;
	pe[1] = 0.0;
}

////////////////////////////////////////////////////////////////////////
//
//  Tql2 - Determine Eigensystem of Symmetric Matrix
//
//  Purpose:	To finds the eigenvalues and eigenvectors of a symmetric
//				tridiagonal matrix by the QL method.
//
//  Call:		int Tql2
//				(
//					int nm,
//					int n.
//					double *pd,
//					double *pe,
//					double *pz,
//					double *perr
//				)
//
//  Where:      nm is the row dimension of the two-dimensional
//                array parameters.
//				n is the matrix order. It must be greater than one.
//				pd is a pointer to a vector containing the diagonal
//				  elements of the tridiagonal matrix.
//				pe is a pointer to the subdiagonal elements of the
//				  tridiagonal matrix in its last n-1 positions are to
//				  be returned. pe[1] is ignored.
//				pz is a pointer to a vector where the orthogonal
//				  transformation matrix produced in the reduction are to
//				  be returned. The matrix must be the identity matrix on
//				  input to this function if the matrix eigenvectors are
//				  to be calculated.
//
//  Return:		Zero if successful; otherwise the index of the first
//				eigenvalue that failed to converge.
//
//  Result:		pd will contain the matrix eigenvalues in ascending
//				  order on completion. If the function fails, the
//				  eigenvalues will be correct but unordered for indices
//				  1, ..., ierr - 1, where ierr is the index of the first
//				  eigenvalue that failed to converge
//				pz will contain the orthonormal eigenvectors of the
//				  matrix. If the function fails, pz will contain the
//				  eigenvectors associated with the stored eigenvalues.
//
//  Note:		This function is a C++ translation of the EISPACK
//				function TQL2.
//
////////////////////////////////////////////////////////////////////////

int BlockLanczos::Tql2( int nm, int n, double *pd, double *pe,
		double *pz )
{
	double c, c2, c3;
	double dl1, el1;
	double f, g, h;
	double p, r, s, s2;
	double tst1, tst2;
	int ii;
	int j;
	int i, k, l, m;		// Loop indices
	int l1, l2;
	int mml;

	for (i = 2; i <= n; i++)
		pe[i - 1] = pe[i];

	f = 0.0;
	tst1 = 0.0;
	pe[n] = 0.0;

	for (l = 1; l <= n; l++)
	{
		j = 0;
		h = fabs(pd[l]) + fabs(pe[l]);

		if (tst1 < h)
			tst1 = h;

		// Look for small sub-diagonal element. (Because pe[n] is always
		// zero, this loop must terminate.)
		for (m = l; m <= n; m++)
		{
			tst2 = tst1 + fabs(pe[m]);

			if (tst2 == tst1)
				break;
		}

		if (m == l)
			goto _220;

_130:
		if (j++ == 30)	// Check for convergence failure (30 iterations)
		{
			return l;	// Indicate failure
		}

		// Form shift
		l1 = l + 1;
		l2 = l1 + 1;
		g = pd[l];
		p = (pd[l1] - g) / (2.0 * pe[l]);
		r = CalcHypotenuse(p, 1.0);

		pd[l] = pe[l] / (p + dsign(r, p));
		pd[l1] = pe[l] * (p + dsign(r, p));
		dl1 = pd[l1];
		h = g - pd[l];

		if (l2 <= n)
		{
			for (i = l2; i <= n; i++)
				pd[i] -= h;
		}

		f += h;

		// QL transformation
		p = pd[m];
		c = 1.0;
		c2 = c;
		el1 = pe[l1];
		s = 0.0;
		mml = m - l;

		// For i=m-1 step -1 until l do
		for (ii = 1; ii <= mml; ii++)
		{
			c3 = c2;
			c2 = c;
            s2 = s;
			i = m - ii;
			g = c * pe[i];
			h = c * p;

			r = CalcHypotenuse(p, pe[i]);

			pe[i + 1] = s * r;

			s = pe[i] / r;
			c = p / r;
			p = c * pd[i] - s * g;
			pd[i + 1] = h + s * (c * g + s * pd[i]);

			// Form vector
			for (k = 1; k <= n; k++)
			{
				h = pz[(i + 1) * nm + k];
				pz[(i + 1) * nm + k] = s * pz[i * nm + k] + c * h;
				pz[i * nm + k] = c * pz[i * nm + k] - s * h;
			}
		}

		p = -s * s2 * c3 * el1 * pe[l] / dl1;
		pe[l] = s * p;
		pd[l] = c * p;
		tst2 = tst1 + fabs(pe[l]);

		if (tst2 > tst1)
			goto _130;
_220:
		pd[l] += f;
	}

	// Order eigenvalues and eigenvectors
	for (ii = 2; ii <= n; ii++)
	{
		i = ii - 1;
		k = i;
		p = pd[i];

		for (j = ii; j <= n; j++)
		{
			if (pd[j] < p)
			{
				k = j;
				p = pd[j];
			}
		}

		if (k != i)
		{
			pd[k] = pd[i];
			pd[i] = p;

			for (j = 1; j <= n; j++)
			{
				// Swap elements
				p = pz[i * nm + j];
				pz[i * nm + j] = pz[k * nm + j];
				pz[k * nm + j] = p;
			}
		}
	}

	return 0;	// Indicate success
}

////////////////////////////////////////////////////////////////////////
//
//  CalcHypotenuse - Calculate Hypotenuse
//
//  Purpose:	To calculate the hypotenuse of a right-angle triangle
//				without overflow or destructive underflow.
//
//  Call:		double Pythag
//				(
//					double a,
//					double b
//				)
//
//  Where:		a is the length of a side of a right-angle triangle.
//				b is the length of a side of a right-angle triangle.
//
//  Note:		This function is a C++ translation of the EISPACK
//				function PYTHAG.
//
////////////////////////////////////////////////////////////////////////

double BlockLanczos::CalcHypotenuse( double a, double b )
{
	double p, r, rsq, s, t, u;

	p = __max(fabs(a), fabs(b));

	if (p > 0)
	{
		r = __min(fabs(a), fabs(b)) / p;
		rsq = r * r;

		for ( ; ; )
		{
			t = 4.0 + rsq;

			if (t == 4.0)
				break;

			s = rsq / t;
			u = 1.0 + 2.0 * s;
			p *= u;
			rsq *= (s * s) / (u * u);
		}
	}

	return p;
}

////////////////////////////////////////////////////////////////////////
//
//  Randomize - Randomize Eigenvector
//
//  Purpose:	To randomize an eigenvector.
//
//  Call:		void Randomize
//				(
//					int k
//				)
//
//  Where:		k is the eigenvector array index.
//
//  Note:		This function computes and stores a sequence of order
//				pseudorandom numbers in the kth column of the order x
//				eva_size array ppvec. It generates two sequences of
//			    pseudorandom numbers, filling an array with one sequence
//				and using the second to access the array in a random
//				fashion.
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::Randomize( int k )
{
	double t[101];	// Temporary array
	int ft;			// Temporary value
	int i;			// Loop index
	int m;			// Array index
	int x1;			// Temporary number

	// Magic numbers
	static const int magic_a = 6821;
	static const int magic_c = 5327;
	static int magic_f1 = 71416;
	static int magic_f2 = 27183;
	static int magic_x0 = 5328;

	for (i = 1; i <= 100; i++)
	{
		x1 = magic_a * magic_x0 + magic_c;

		if (x1 >= 10000)
			x1 -= 10000;

		t[i] = (double) x1 / 9999.0 - 0.5;

		magic_x0 = x1;
	}

	for (i = 1; i <= order; i++)
	{
		ft = magic_f1 + magic_f2;

		if (ft >= 1000000)
			ft -= 1000000;

		magic_f1 = magic_f2;
		magic_f2 = ft;		

		m = ft / 10000 + 1;

		ppvec[k][i] = t[m];

		x1 = magic_a * magic_x0 + magic_c;

		if (x1 >= 10000)
			x1 -= 10000;

		t[m] = (double) x1 / 9999.0 - 0.5;

		magic_x0 = x1;
	}
}

////////////////////////////////////////////////////////////////////////
//
//  Release - Release Memory
//
//  Purpose:	To release memory allocated to the eigensolver.
//
//  Call:		void Release()
//
////////////////////////////////////////////////////////////////////////

void BlockLanczos::Release()
{
	int i;	// Loop index

	if (pdiag != NULL)
	{
		delete [] pdiag;	// Release array diagonal elements vector
		pdiag = NULL;
	}

	if (peuc != NULL)
	{
		delete [] peuc;		// Release Euclidean length array
		peuc = NULL;
	}

	if (pmat != NULL)
	{
		delete [] pmat;		// Release matrix array
		pmat = NULL;
	}

	if (pu != NULL)
	{
		delete [] pu;		// Release input vector
		pu = NULL;
	}

	if (pv != NULL)
	{
		delete [] pv;		// Release output vector
		pv = NULL;
	}

	if (pval != NULL)
	{
		delete [] pval;		// Release eigenvalue array
		pval = NULL;
	}

	if (ppvec != NULL)
	{
		// Release eigenvector array
		for (i = 1; i <= eva_size; i++)
		{
			if (ppvec[i] != NULL)
			{
				delete [] ppvec[i];		// Release eigenvector
			}
		}

		delete [] ppvec;	// Release eigenvector pointers array
		ppvec = NULL;
	}
}
