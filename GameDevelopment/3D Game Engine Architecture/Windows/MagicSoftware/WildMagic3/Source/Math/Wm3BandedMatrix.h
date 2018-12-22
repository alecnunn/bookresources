// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BANDEDMATRIX_H
#define WM3BANDEDMATRIX_H

#include "Wm3System.h"

namespace Wm3
{

template <class Real>
class BandedMatrix
{
public:
    BandedMatrix (int iSize, int iLBands, int iUBands);
    BandedMatrix (const BandedMatrix& rkM);
    ~BandedMatrix ();

    BandedMatrix& operator= (const BandedMatrix& rkM);

    int GetSize () const;
    int GetLBands () const;
    int GetUBands () const;

    Real* GetDBand ();
    const Real* GetDBand () const;

    int GetLBandMax (int i) const;  // LBand(i):  0 <= index < LBandMax
    Real* GetLBand (int i);
    const Real* GetLBand (int i) const;

    int GetUBandMax (int i) const;  // UBand(i):  0 <= index < UBandMax
    Real* GetUBand (int i);
    const Real* GetUBand (int i) const;

    Real& operator() (int iRow, int iCol);
    Real operator() (int iRow, int iCol) const;

    void SetZero ();
    void SetIdentity ();

private:
    void Allocate ();
    void Deallocate ();

    int m_iSize, m_iLBands, m_iUBands;
    Real* m_afDBand;
    Real** m_aafLBand;
    Real** m_aafUBand;
};

#include "Wm3BandedMatrix.inl"

typedef BandedMatrix<float> BandedMatrixf;
typedef BandedMatrix<double> BandedMatrixd;

}

#endif
