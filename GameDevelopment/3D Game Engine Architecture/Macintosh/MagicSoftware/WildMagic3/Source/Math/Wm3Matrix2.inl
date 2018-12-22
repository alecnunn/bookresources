// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (bool bZero)
{
    if ( bZero )
        MakeZero();
    else
        MakeIdentity();
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (const Matrix2& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,4*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (Real fM00, Real fM01, Real fM10, Real fM11)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = fM01;
    m_afEntry[2] = fM10;
    m_afEntry[3] = fM11;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (const Real afEntry[4], bool bRowMajor)
{
    if ( bRowMajor )
    {
        memcpy(m_afEntry,afEntry,4*sizeof(Real));
    }
    else
    {
        m_afEntry[0] = afEntry[0];
        m_afEntry[1] = afEntry[2];
        m_afEntry[2] = afEntry[1];
        m_afEntry[3] = afEntry[3];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (const Vector2<Real>& rkU, const Vector2<Real>& rkV,
    bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkV[0];
        m_afEntry[2] = rkU[1];
        m_afEntry[3] = rkV[1];
    }
    else
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkU[1];
        m_afEntry[2] = rkV[0];
        m_afEntry[3] = rkV[1];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (const Vector2<Real>* akV, bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[1][0];
        m_afEntry[2] = akV[0][1];
        m_afEntry[3] = akV[1][1];
    }
    else
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[0][1];
        m_afEntry[2] = akV[1][0];
        m_afEntry[3] = akV[1][1];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (Real fM00, Real fM11)
{
    MakeDiagonal(fM00,fM11);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (Real fAngle)
{
    FromAngle(fAngle);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::Matrix2 (const Vector2<Real>& rkU, const Vector2<Real>& rkV)
{
    MakeTensorProduct(rkU,rkV);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::operator const Real* () const
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>::operator Real* ()
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* Matrix2<Real>::operator[] (int iRow) const
{
    return &m_afEntry[2*iRow];
}
//----------------------------------------------------------------------------
template <class Real>
Real* Matrix2<Real>::operator[] (int iRow)
{
    return &m_afEntry[2*iRow];
}
//----------------------------------------------------------------------------
template <class Real>
Real Matrix2<Real>::operator() (int iRow, int iCol) const
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Matrix2<Real>::operator() (int iRow, int iCol)
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class Real>
int Matrix2<Real>::I (int iRow, int iCol)
{
    assert( 0 <= iRow && iRow < 2 && 0 <= iCol && iCol < 2 );
    return iCol + 2*iRow;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::MakeZero ()
{
    memset(m_afEntry,0,4*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::MakeIdentity ()
{
    m_afEntry[0] = (Real)1.0;
    m_afEntry[1] = (Real)0.0;
    m_afEntry[2] = (Real)0.0;
    m_afEntry[3] = (Real)1.0;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::MakeDiagonal (Real fM00, Real fM11)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = (Real)0.0;
    m_afEntry[2] = (Real)0.0;
    m_afEntry[3] = fM11;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::FromAngle (Real fAngle)
{
    m_afEntry[0] = Math<Real>::Cos(fAngle);
    m_afEntry[2] = Math<Real>::Sin(fAngle);
    m_afEntry[1] = -m_afEntry[2];
    m_afEntry[3] =  m_afEntry[0];
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::MakeTensorProduct (const Vector2<Real>& rkU,
    const Vector2<Real>& rkV)
{
    m_afEntry[0] = rkU[0]*rkV[0];
    m_afEntry[1] = rkU[0]*rkV[1];
    m_afEntry[2] = rkU[1]*rkV[0];
    m_afEntry[3] = rkU[1]*rkV[1];
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::SetRow (int iRow, const Vector2<Real>& rkV)
{
    assert( 0 <= iRow && iRow < 2 );
    for (int iCol = 0, i = 2*iRow; iCol < 2; iCol++, i++)
        m_afEntry[i] = rkV[iCol];
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Matrix2<Real>::GetRow (int iRow) const
{
    assert( 0 <= iRow && iRow < 2 );
    Vector2<Real> kV;
    for (int iCol = 0, i = 2*iRow; iCol < 2; iCol++, i++)
        kV[iCol] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::SetColumn (int iCol, const Vector2<Real>& rkV)
{
    assert( 0 <= iCol && iCol < 2 );
    for (int iRow = 0, i = iCol; iRow < 2; iRow++, i += 2)
        m_afEntry[i] = rkV[iRow];
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Matrix2<Real>::GetColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 2 );
    Vector2<Real> kV;
    for (int iRow = 0, i = iCol; iRow < 2; iRow++, i += 2)
        kV[iRow] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::GetColumnMajor (Real* afCMajor) const
{
    for (int iRow = 0, i = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            afCMajor[i++] = m_afEntry[I(iCol,iRow)];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>& Matrix2<Real>::operator= (const Matrix2& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,4*sizeof(Real));
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
int Matrix2<Real>::CompareArrays (const Matrix2& rkM) const
{
    return memcmp(m_afEntry,rkM.m_afEntry,4*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix2<Real>::operator== (const Matrix2& rkM) const
{
    return CompareArrays(rkM) == 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix2<Real>::operator!= (const Matrix2& rkM) const
{
    return CompareArrays(rkM) != 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix2<Real>::operator<  (const Matrix2& rkM) const
{
    return CompareArrays(rkM) < 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix2<Real>::operator<= (const Matrix2& rkM) const
{
    return CompareArrays(rkM) <= 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix2<Real>::operator>  (const Matrix2& rkM) const
{
    return CompareArrays(rkM) > 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix2<Real>::operator>= (const Matrix2& rkM) const
{
    return CompareArrays(rkM) >= 0;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::operator+ (const Matrix2& rkM) const
{
    Matrix2 kSum;
    for (int i = 0; i < 4; i++)
        kSum.m_afEntry[i] = m_afEntry[i] + rkM.m_afEntry[i];
    return kSum;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::operator- (const Matrix2& rkM) const
{
    Matrix2 kDiff;
    for (int i = 0; i < 4; i++)
        kDiff.m_afEntry[i] = m_afEntry[i] - rkM.m_afEntry[i];
    return kDiff;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::operator* (const Matrix2& rkM) const
{
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (Real)0.0;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kProd.m_afEntry[i] +=
                    m_afEntry[I(iRow,iMid)]*rkM.m_afEntry[I(iMid,iCol)];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::operator* (Real fScalar) const
{
    Matrix2 kProd;
    for (int i = 0; i < 4; i++)
        kProd.m_afEntry[i] = fScalar*m_afEntry[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::operator/ (Real fScalar) const
{
    Matrix2 kQuot;
    int i;

    if ( fScalar != (Real)0.0 )
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            kQuot.m_afEntry[i] = fInvScalar*m_afEntry[i];
    }
    else
    {
        for (i = 0; i < 4; i++)
            kQuot.m_afEntry[i] = Math<Real>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::operator- () const
{
    Matrix2 kNeg;
    for (int i = 0; i < 4; i++)
        kNeg.m_afEntry[i] = -m_afEntry[i];
    return kNeg;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>& Matrix2<Real>::operator+= (const Matrix2& rkM)
{
    for (int i = 0; i < 4; i++)
        m_afEntry[i] += rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>& Matrix2<Real>::operator-= (const Matrix2& rkM)
{
    for (int i = 0; i < 4; i++)
        m_afEntry[i] -= rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>& Matrix2<Real>::operator*= (Real fScalar)
{
    for (int i = 0; i < 4; i++)
        m_afEntry[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real>& Matrix2<Real>::operator/= (Real fScalar)
{
    int i;

    if ( fScalar != (Real)0.0 )
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        for (i = 0; i < 4; i++)
            m_afEntry[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < 4; i++)
            m_afEntry[i] = Math<Real>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> Matrix2<Real>::operator* (const Vector2<Real>& rkV) const
{
    Vector2<Real> kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        kProd[iRow] = (Real)0.0;
        for (int iCol = 0; iCol < 2; iCol++)
            kProd[iRow] += m_afEntry[I(iRow,iCol)]*rkV[iCol];
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::Transpose () const
{
    Matrix2 kTranspose;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
            kTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::TransposeTimes (const Matrix2& rkM) const
{
    // P = A^T*B, P[r][c] = sum_m A[m][r]*B[m][c]
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (Real)0.0;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kProd.m_afEntry[i] +=
                    m_afEntry[I(iMid,iRow)]*rkM.m_afEntry[I(iMid,iCol)];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::TimesTranspose (const Matrix2& rkM) const
{
    // P = A*B^T, P[r][c] = sum_m A[r][m]*B[c][m]
    Matrix2 kProd;
    for (int iRow = 0; iRow < 2; iRow++)
    {
        for (int iCol = 0; iCol < 2; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (Real)0.0;
            for (int iMid = 0; iMid < 2; iMid++)
            {
                kProd.m_afEntry[i] +=
                    m_afEntry[I(iRow,iMid)]*rkM.m_afEntry[I(iCol,iRow)];
            }
        }
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::Inverse () const
{
    Matrix2 kInverse;

    Real fDet = m_afEntry[0]*m_afEntry[3] - m_afEntry[1]*m_afEntry[2];
    if ( Math<Real>::FAbs(fDet) > Math<Real>::ZERO_TOLERANCE )
    {
        Real fInvDet = ((Real)1.0)/fDet;
        kInverse[0][0] =  m_afEntry[3]*fInvDet;
        kInverse[0][1] = -m_afEntry[1]*fInvDet;
        kInverse[1][0] = -m_afEntry[2]*fInvDet;
        kInverse[1][1] =  m_afEntry[0]*fInvDet;
    }
    else
    {
        kInverse.MakeZero();
    }

    return kInverse;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> Matrix2<Real>::Adjoint () const
{
    return Matrix2(
         m_afEntry[3],-m_afEntry[1],
        -m_afEntry[2], m_afEntry[0]);
}
//----------------------------------------------------------------------------
template <class Real>
Real Matrix2<Real>::Determinant () const
{
    return m_afEntry[0]*m_afEntry[3] - m_afEntry[1]*m_afEntry[2];
}
//----------------------------------------------------------------------------
template <class Real>
Real Matrix2<Real>::QForm (const Vector2<Real>& rkU,
    const Vector2<Real>& rkV) const
{
    return rkU.Dot((*this)*rkV);
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::ToAngle (Real& rfAngle) const
{
    // assert:  matrix is a rotation
    rfAngle = Math<Real>::ATan2(m_afEntry[2],m_afEntry[0]);
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1], then orthonormal output matrix is Q = [q0|q1],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    Real fInvLength = Math<Real>::InvSqrt(m_afEntry[0]*m_afEntry[0] +
        m_afEntry[2]*m_afEntry[2]);

    m_afEntry[0] *= fInvLength;
    m_afEntry[2] *= fInvLength;

    // compute q1
    Real fDot0 = m_afEntry[0]*m_afEntry[1] + m_afEntry[2]*m_afEntry[3];
    m_afEntry[1] -= fDot0*m_afEntry[0];
    m_afEntry[3] -= fDot0*m_afEntry[2];

    fInvLength = Math<Real>::InvSqrt(m_afEntry[1]*m_afEntry[1] +
        m_afEntry[3]*m_afEntry[3]);

    m_afEntry[1] *= fInvLength;
    m_afEntry[3] *= fInvLength;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix2<Real>::EigenDecomposition (Matrix2& rkRot, Matrix2& rkDiag) const
{
    Real fTrace = m_afEntry[0] + m_afEntry[3];
    Real fDiff = m_afEntry[0] - m_afEntry[3];
    Real fDiscr = Math<Real>::Sqrt(fDiff*fDiff +
        ((Real)4.0)*m_afEntry[1]*m_afEntry[1]);
    Real fEVal0 = ((Real)0.5)*(fTrace-fDiscr);
    Real fEVal1 = ((Real)0.5)*(fTrace+fDiscr);
    rkDiag.MakeDiagonal(fEVal0,fEVal1);

    Vector2<Real> kRow0(m_afEntry[0]-fEVal0,m_afEntry[1]);
    Vector2<Real> kRow1(m_afEntry[1],m_afEntry[3]-fEVal0);
    Real fLength0 = kRow0.Length();
    Real fLength1 = kRow1.Length();

    if ( fLength0 > fLength1 )
    {
        if ( fLength0 > Math<Real>::ZERO_TOLERANCE )
        {
            rkRot.m_afEntry[0] = kRow0[1];
            rkRot.m_afEntry[1] = kRow0[0];
            rkRot.m_afEntry[2] = -kRow0[0];
            rkRot.m_afEntry[3] = kRow0[1];
        }
        else
        {
            rkRot.MakeIdentity();
        }
    }
    else
    {
        if ( fLength1 > Math<Real>::ZERO_TOLERANCE )
        {
            rkRot.m_afEntry[0] = kRow1[1];
            rkRot.m_afEntry[1] = kRow1[0];
            rkRot.m_afEntry[2] = -kRow1[0];
            rkRot.m_afEntry[3] = kRow1[1];
        }
        else
        {
            rkRot.MakeIdentity();
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix2<Real> operator* (Real fScalar, const Matrix2<Real>& rkM)
{
    return rkM*fScalar;
}
//----------------------------------------------------------------------------
template <class Real>
Vector2<Real> operator* (const Vector2<Real>& rkV, const Matrix2<Real>& rkM)
{
    return Vector2<Real>(
        rkV[0]*rkM[0][0] + rkV[1]*rkM[1][0],
        rkV[0]*rkM[0][1] + rkV[1]*rkM[1][1]);
}
//----------------------------------------------------------------------------
