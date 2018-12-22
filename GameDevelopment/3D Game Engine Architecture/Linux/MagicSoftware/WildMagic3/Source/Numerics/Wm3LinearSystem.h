// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3LINEARSYSTEM_H
#define WM3LINEARSYSTEM_H

#include "Wm3BandedMatrix.h"
#include "Wm3GMatrix.h"
#include <map>

namespace Wm3
{

template <class Real>
class WM3_ITEM LinearSystem
{
public:
    // construction
    LinearSystem ();

    // 2x2 and 3x3 systems (avoids overhead of Gaussian elimination)
    bool Solve2 (const Real aafA[2][2], const Real afB[2], Real afX[2]);
    bool Solve3 (const Real aafA[3][3], const Real afB[3], Real afX[3]);

    // Input:
    //     A[iSize][iSize], entries are A[row][col]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     InvA[iSize][iSize], inverse matrix
    bool Inverse (const GMatrix<Real>& rkA, GMatrix<Real>& rkInvA);

    // Input:
    //     A[iSize][iSize] coefficient matrix, entries are A[row][col]
    //     B[iSize] vector, entries are B[row]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     X[iSize] is solution X to AX = B
    bool Solve (const GMatrix<Real>& rkA, const Real* afB, Real* afX);

    // Input:
    //     Matrix is tridiagonal.
    //     Lower diagonal A[iSize-1]
    //     Main  diagonal B[iSize]
    //     Upper diagonal C[iSize-1]
    //     Right-hand side R[iSize]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     U[iSize] is solution
    bool SolveTri (int iSize, Real* afA, Real* afB, Real* afC, Real* afR,
        Real* afU);

    // Input:
    //     Matrix is tridiagonal.
    //     Lower diagonal is constant, A
    //     Main  diagonal is constant, B
    //     Upper diagonal is constant, C
    //     Right-hand side Rr[iSize]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     U[iSize] is solution
    bool SolveConstTri (int iSize, Real fA, Real fB, Real fC, Real* afR,
        Real* afU);

    // Input:
    //     A[iSize][iSize] symmetric matrix, entries are A[row][col]
    //     B[iSize] vector, entries are B[row]
    // Output:
    //     return value is TRUE if successful, FALSE if (nearly) singular
    //     X[iSize] is solution X to AX = B
    bool SolveSymmetric (const GMatrix<Real>& rkA, const Real* afB,
        Real* afX);

    // Input:
    //     A[iSize][iSize] symmetric matrix, entries are A[row][col]
    // Output:
    //     return value is TRUE if successful, FALSE if algorithm failed
    //     InvA[iSize][iSize], inverse matrix
    bool SymmetricInverse (const GMatrix<Real>& rkA, GMatrix<Real>& rkInvA);

    // Solution using the conjugate gradient method.
    // Input:
    //    A[iSize][iSize] symmetrix matrix, entries are A[row][col]
    //    B[iSize] vector, entries are B[row]
    // Output:
    //    X[iSize] is the solution x to Ax = B
    bool SolveSymmetricCG (const GMatrix<Real>& rkA, const Real* afB,
        Real* afX);

    // Conjugate gradient method for sparse, symmetric matrices.
    // Input:
    //    The nonzero entries of the symmetrix matrix A are stored in a map
    //    whose keys are pairs (i,j) and whose values are real numbers.  The
    //    pair (i,j) is the location of the value in the array.  Only one of
    //    (i,j) and (j,i) should be stored since A is symmetric.  The code
    //    assumes this is how you set up A.  The column vector B is stored as
    //    an array of contiguous values.
    // Output:
    //    X[iSize] is the solution x to Ax = B
    typedef std::map<std::pair<int,int>,Real> SparseMatrix;
    bool SolveSymmetricCG (int iSize, const SparseMatrix& rkA,
        const Real* afB, Real* afX);

    // solve banded matrix systems
    // Input:
    //     A, a banded matrix
    //     B[iSize] vector, entries are B[row]
    // Output:
    //     return value is TRUE if successful, FALSE if pivoting failed
    //     X[iSize] is solution X to AX = B
    bool SolveBanded (const BandedMatrix<Real>& rkA, const Real* afB,
        Real* afX);

    // invert a banded matrix
    // Input:
    //     A, a banded matrix
    // Output:
    //     return value is TRUE if the inverse exists, FALSE otherwise
    //     InvA, the inverse of A
    bool Invert (const BandedMatrix<Real>& rkA, GMatrix<Real>& rkInvA);

    // tolerance for linear system solving
    Real ZeroTolerance;  // default = Math<Real>::ZERO_TOLERANCE

private:
    // support for the conjugate gradient method for standard arrays
    Real Dot (int iSize, const Real* afU, const Real* afV);
    void Multiply (const GMatrix<Real>& rkA, const Real* afX, Real* afProd);
    void UpdateX (int iSize, Real* afX, Real fAlpha, const Real* afP);
    void UpdateR (int iSize, Real* afR, Real fAlpha, const Real* afW);
    void UpdateP (int iSize, Real* afP, Real fBeta, const Real* afR);

    // support for the conjugate gradient method for sparse arrays
    void Multiply (int iSize, const SparseMatrix& rkA, const Real* afX,
        Real* afProd);

    // support for banded matrices
    bool ForwardEliminate (int iReduceRow, BandedMatrix<Real>& rkA,
        Real* afB);
    bool ForwardEliminate (int iReduceRow, BandedMatrix<Real>& rkA,
        GMatrix<Real>& rkB);
    void BackwardEliminate (int iReduceRow, BandedMatrix<Real>& rkA,
        GMatrix<Real>& rkB);
};

typedef LinearSystem<float> LinearSystemf;
typedef LinearSystem<double> LinearSystemd;

}

#endif
