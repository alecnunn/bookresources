////////////////////////////////////////////////////////////////////////
//
//  BLOCKLANCZOS.H - Iterative Block Lanczos Eigensolver Class Include
//                   File
//
//  Version:    4.00A
//
//  History:    00/05/30 - Created.
//				01/02/01 - Added neg_flag data member.
//						 - Modified Init and puser function prototypes.
//				01/04/02 - Fixed GetEigenvector error.
//				03/03/23 - Revised copyright notice.
//
//  Compilers:  Microsoft Visual C/C++ Professional V6.0
//
//  Reference:  Underwood, R. R. 1975. An Iterative Block Lanczos Method
//              for the Solution of Large Sparse Symmetric
//              Eigenproblems, Technical Report STAN-CS-75-495,
//              Department of Computer Science, Stanford University,
//              Stanford, CA.
//
//  Author:     Ian Ashdown, P. Eng.
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
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define dsign(a,b) (b >= 0.0 ? a : -a)

class BlockLanczos
{
    private:
		bool neg_flag;		// Negative matrix flag
        double acc_err;     // Accumulated error
        double prec;        // Precision
        double *pdiag;      // Array diagonal elements vector
        double *peuc;       // Euclidean length array pointer
        double *pmat;       // Block matrix array pointer
        double *pu;         // Input vector pointer
        double *pv;         // Output vector pointer
        double *pval;       // Eigenvalue array pointer
        double **ppvec;     // Eigenvector pointer array pointer
        int curr_size;      // Current block size
        int eva_size;       // Eigenvector vector array size
        int init_size;      // Initial block size
        int max_eig;        // Maximum number of eigenvalues
        int max_iter;       // Maximum number of iterations
        int num_calc;       // Number of calculated eigenvalues
        int num_conv;       // Number of converged eigenvalues
        int num_iter;       // Number of iterations
        int num_mvm;        // Number of matrix-vector multiplications
        int num_steps;      // Number of steps
        int order;          // Matrix order

        // User-specified function pointer
        void (*puser)( double *, double *, int, bool );

        bool CalcDiagonal();
        bool CalcEigen( int, int );
        double CalcHypotenuse( double, double );
        int Tql2( int, int, double *, double *, double * );
        void CalcNorm();
        void ChooseBlockSize();
        void PerformMethod();
        void Randomize( int );
        void Reorthogonalize( int );
        void Rotate( int, int );
        void TestConvergence();
        void Tred2( int, int, double *, double *, double *, double * );

    public:
        BlockLanczos()
        {
			neg_flag = true;
            acc_err = 0.0;
            prec = 1.0e-3;
            pdiag = NULL;
            peuc = NULL;
            pmat = NULL;
            pu = NULL;
            pv = NULL;
            pval = NULL;
            ppvec = NULL;
            curr_size = 0;
            eva_size = 0;
            init_size = 0;
            max_eig = 0;
            max_iter = 0;
            num_calc = 0;
            num_conv = 0;
            order = 0;
            puser = NULL;
        }

        ~BlockLanczos() { Release(); }

        bool Calculate( int * );
        bool Init( bool, int, int, int, int, int, void (*puser)
				( double *, double *, int, bool ), double );
        double GetEigenvalue( int i )
		{ return neg_flag == true ? -pval[i + 1] : pval[i + 1]; }
        double *GetEigenvector( int i ) { return (ppvec[i + 1] + 1); }
        void Release();
};
