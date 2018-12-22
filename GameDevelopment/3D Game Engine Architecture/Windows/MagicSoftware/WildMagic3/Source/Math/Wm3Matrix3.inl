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
Matrix3<Real>::Matrix3 (bool bZero)
{
    if ( bZero )
        MakeZero();
    else
        MakeIdentity();
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (const Matrix3& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,9*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (Real fM00, Real fM01, Real fM02, Real fM10, Real fM11,
    Real fM12, Real fM20, Real fM21, Real fM22)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = fM01;
    m_afEntry[2] = fM02;
    m_afEntry[3] = fM10;
    m_afEntry[4] = fM11;
    m_afEntry[5] = fM12;
    m_afEntry[6] = fM20;
    m_afEntry[7] = fM21;
    m_afEntry[8] = fM22;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (const Real afEntry[9], bool bRowMajor)
{
    if ( bRowMajor )
    {
        memcpy(m_afEntry,afEntry,9*sizeof(Real));
    }
    else
    {
        m_afEntry[0] = afEntry[0];
        m_afEntry[1] = afEntry[3];
        m_afEntry[2] = afEntry[6];
        m_afEntry[3] = afEntry[1];
        m_afEntry[4] = afEntry[4];
        m_afEntry[5] = afEntry[7];
        m_afEntry[6] = afEntry[2];
        m_afEntry[7] = afEntry[5];
        m_afEntry[8] = afEntry[8];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>& rkU, const Vector3<Real>& rkV,
    const Vector3<Real>& rkW, bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkV[0];
        m_afEntry[2] = rkW[0];
        m_afEntry[3] = rkU[1];
        m_afEntry[4] = rkV[1];
        m_afEntry[5] = rkW[1];
        m_afEntry[6] = rkU[2];
        m_afEntry[7] = rkV[2];
        m_afEntry[8] = rkW[2];
    }
    else
    {
        m_afEntry[0] = rkU[0];
        m_afEntry[1] = rkU[1];
        m_afEntry[2] = rkU[2];
        m_afEntry[3] = rkV[0];
        m_afEntry[4] = rkV[1];
        m_afEntry[5] = rkV[2];
        m_afEntry[6] = rkW[0];
        m_afEntry[7] = rkW[1];
        m_afEntry[8] = rkW[2];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>* akV, bool bColumns)
{
    if ( bColumns )
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[1][0];
        m_afEntry[2] = akV[2][0];
        m_afEntry[3] = akV[0][1];
        m_afEntry[4] = akV[1][1];
        m_afEntry[5] = akV[2][1];
        m_afEntry[6] = akV[0][2];
        m_afEntry[7] = akV[1][2];
        m_afEntry[8] = akV[2][2];
    }
    else
    {
        m_afEntry[0] = akV[0][0];
        m_afEntry[1] = akV[0][1];
        m_afEntry[2] = akV[0][2];
        m_afEntry[3] = akV[1][0];
        m_afEntry[4] = akV[1][1];
        m_afEntry[5] = akV[1][2];
        m_afEntry[6] = akV[2][0];
        m_afEntry[7] = akV[2][1];
        m_afEntry[8] = akV[2][2];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (Real fM00, Real fM11, Real fM22)
{
    MakeDiagonal(fM00,fM11,fM22);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>& rkAxis, Real fAngle)
{
    FromAxisAngle(rkAxis,fAngle);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::Matrix3 (const Vector3<Real>& rkU, const Vector3<Real>& rkV)
{
    MakeTensorProduct(rkU,rkV);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::operator const Real* () const
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>::operator Real* ()
{
    return m_afEntry;
}
//----------------------------------------------------------------------------
template <class Real>
const Real* Matrix3<Real>::operator[] (int iRow) const
{
    return &m_afEntry[3*iRow];
}
//----------------------------------------------------------------------------
template <class Real>
Real* Matrix3<Real>::operator[] (int iRow)
{
    return &m_afEntry[3*iRow];
}
//----------------------------------------------------------------------------
template <class Real>
Real Matrix3<Real>::operator() (int iRow, int iCol) const
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class Real>
Real& Matrix3<Real>::operator() (int iRow, int iCol)
{
    return m_afEntry[I(iRow,iCol)];
}
//----------------------------------------------------------------------------
template <class Real>
int Matrix3<Real>::I (int iRow, int iCol)
{
    assert( 0 <= iRow && iRow < 3 && 0 <= iCol && iCol < 3 );
    return iCol + 3*iRow;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::MakeZero ()
{
    memset(m_afEntry,0,9*sizeof(Real));
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::MakeIdentity ()
{
    m_afEntry[0] = (Real)1.0;
    m_afEntry[1] = (Real)0.0;
    m_afEntry[2] = (Real)0.0;
    m_afEntry[3] = (Real)0.0;
    m_afEntry[4] = (Real)1.0;
    m_afEntry[5] = (Real)0.0;
    m_afEntry[6] = (Real)0.0;
    m_afEntry[7] = (Real)0.0;
    m_afEntry[8] = (Real)1.0;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::MakeDiagonal (Real fM00, Real fM11, Real fM22)
{
    m_afEntry[0] = fM00;
    m_afEntry[1] = (Real)0.0;
    m_afEntry[2] = (Real)0.0;
    m_afEntry[3] = (Real)0.0;
    m_afEntry[4] = fM11;
    m_afEntry[5] = (Real)0.0;
    m_afEntry[6] = (Real)0.0;
    m_afEntry[7] = (Real)0.0;
    m_afEntry[8] = fM22;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::FromAxisAngle (const Vector3<Real>& rkAxis,
    Real fAngle)
{
    Real fCos = Math<Real>::Cos(fAngle);
    Real fSin = Math<Real>::Sin(fAngle);
    Real fOneMinusCos = ((Real)1.0)-fCos;
    Real fX2 = rkAxis[0]*rkAxis[0];
    Real fY2 = rkAxis[1]*rkAxis[1];
    Real fZ2 = rkAxis[2]*rkAxis[2];
    Real fXYM = rkAxis[0]*rkAxis[1]*fOneMinusCos;
    Real fXZM = rkAxis[0]*rkAxis[2]*fOneMinusCos;
    Real fYZM = rkAxis[1]*rkAxis[2]*fOneMinusCos;
    Real fXSin = rkAxis[0]*fSin;
    Real fYSin = rkAxis[1]*fSin;
    Real fZSin = rkAxis[2]*fSin;
    
    m_afEntry[0] = fX2*fOneMinusCos+fCos;
    m_afEntry[1] = fXYM-fZSin;
    m_afEntry[2] = fXZM+fYSin;
    m_afEntry[3] = fXYM+fZSin;
    m_afEntry[4] = fY2*fOneMinusCos+fCos;
    m_afEntry[5] = fYZM-fXSin;
    m_afEntry[6] = fXZM-fYSin;
    m_afEntry[7] = fYZM+fXSin;
    m_afEntry[8] = fZ2*fOneMinusCos+fCos;

    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::MakeTensorProduct (const Vector3<Real>& rkU,
    const Vector3<Real>& rkV)
{
    m_afEntry[0] = rkU[0]*rkV[0];
    m_afEntry[1] = rkU[0]*rkV[1];
    m_afEntry[2] = rkU[0]*rkV[2];
    m_afEntry[3] = rkU[1]*rkV[0];
    m_afEntry[4] = rkU[1]*rkV[1];
    m_afEntry[5] = rkU[1]*rkV[2];
    m_afEntry[6] = rkU[2]*rkV[0];
    m_afEntry[7] = rkU[2]*rkV[1];
    m_afEntry[8] = rkU[2]*rkV[2];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::SetRow (int iRow, const Vector3<Real>& rkV)
{
    assert( 0 <= iRow && iRow < 3 );
    for (int iCol = 0, i = 3*iRow; iCol < 3; iCol++, i++)
        m_afEntry[i] = rkV[iCol];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Matrix3<Real>::GetRow (int iRow) const
{
    assert( 0 <= iRow && iRow < 3 );
    Vector3<Real> kV;
    for (int iCol = 0, i = 3*iRow; iCol < 3; iCol++, i++)
        kV[iCol] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::SetColumn (int iCol, const Vector3<Real>& rkV)
{
    assert( 0 <= iCol && iCol < 3 );
    for (int iRow = 0, i = iCol; iRow < 3; iRow++, i += 3)
        m_afEntry[i] = rkV[iRow];
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Matrix3<Real>::GetColumn (int iCol) const
{
    assert( 0 <= iCol && iCol < 3 );
    Vector3<Real> kV;
    for (int iRow = 0, i = iCol; iRow < 3; iRow++, i += 3)
        kV[iRow] = m_afEntry[i];
    return kV;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::GetColumnMajor (Real* afCMajor) const
{
    for (int iRow = 0, i = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            afCMajor[i++] = m_afEntry[I(iCol,iRow)];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::operator= (const Matrix3& rkM)
{
    memcpy(m_afEntry,rkM.m_afEntry,9*sizeof(Real));
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
int Matrix3<Real>::CompareArrays (const Matrix3& rkM) const
{
    return memcmp(m_afEntry,rkM.m_afEntry,9*sizeof(Real));
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::operator== (const Matrix3& rkM) const
{
    return CompareArrays(rkM) == 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::operator!= (const Matrix3& rkM) const
{
    return CompareArrays(rkM) != 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::operator<  (const Matrix3& rkM) const
{
    return CompareArrays(rkM) < 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::operator<= (const Matrix3& rkM) const
{
    return CompareArrays(rkM) <= 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::operator>  (const Matrix3& rkM) const
{
    return CompareArrays(rkM) > 0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::operator>= (const Matrix3& rkM) const
{
    return CompareArrays(rkM) >= 0;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::operator+ (const Matrix3& rkM) const
{
    Matrix3 kSum;
    for (int i = 0; i < 9; i++)
        kSum.m_afEntry[i] = m_afEntry[i] + rkM.m_afEntry[i];
    return kSum;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::operator- (const Matrix3& rkM) const
{
    Matrix3 kDiff;
    for (int i = 0; i < 9; i++)
        kDiff.m_afEntry[i] = m_afEntry[i] - rkM.m_afEntry[i];
    return kDiff;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::operator* (const Matrix3& rkM) const
{
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (Real)0.0;
            for (int iMid = 0; iMid < 3; iMid++)
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
Matrix3<Real> Matrix3<Real>::operator* (Real fScalar) const
{
    Matrix3 kProd;
    for (int i = 0; i < 9; i++)
        kProd.m_afEntry[i] = fScalar*m_afEntry[i];
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::operator/ (Real fScalar) const
{
    Matrix3 kQuot;
    int i;

    if ( fScalar != (Real)0.0 )
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        for (i = 0; i < 9; i++)
            kQuot.m_afEntry[i] = fInvScalar*m_afEntry[i];
    }
    else
    {
        for (i = 0; i < 9; i++)
            kQuot.m_afEntry[i] = Math<Real>::MAX_REAL;
    }

    return kQuot;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::operator- () const
{
    Matrix3 kNeg;
    for (int i = 0; i < 9; i++)
        kNeg.m_afEntry[i] = -m_afEntry[i];
    return kNeg;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::operator+= (const Matrix3& rkM)
{
    for (int i = 0; i < 9; i++)
        m_afEntry[i] += rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::operator-= (const Matrix3& rkM)
{
    for (int i = 0; i < 9; i++)
        m_afEntry[i] -= rkM.m_afEntry[i];
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::operator*= (Real fScalar)
{
    for (int i = 0; i < 9; i++)
        m_afEntry[i] *= fScalar;
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::operator/= (Real fScalar)
{
    int i;

    if ( fScalar != (Real)0.0 )
    {
        Real fInvScalar = ((Real)1.0)/fScalar;
        for (i = 0; i < 9; i++)
            m_afEntry[i] *= fInvScalar;
    }
    else
    {
        for (i = 0; i < 9; i++)
            m_afEntry[i] = Math<Real>::MAX_REAL;
    }

    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> Matrix3<Real>::operator* (const Vector3<Real>& rkV) const
{
    Vector3<Real> kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        kProd[iRow] = 0.0f;
        for (int iCol = 0; iCol < 3; iCol++)
            kProd[iRow] += m_afEntry[I(iRow,iCol)]*rkV[iCol];
            
    }
    return kProd;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::Transpose () const
{
    Matrix3 kTranspose;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
            kTranspose.m_afEntry[I(iRow,iCol)] = m_afEntry[I(iCol,iRow)];
    }
    return kTranspose;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::TransposeTimes (const Matrix3& rkM) const
{
    // P = A^T*B, P[r][c] = sum_m A[m][r]*B[m][c]
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (Real)0.0;
            for (int iMid = 0; iMid < 3; iMid++)
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
Matrix3<Real> Matrix3<Real>::TimesTranspose (const Matrix3& rkM) const
{
    // P = A*B^T, P[r][c] = sum_m A[r][m]*B[c][m]
    Matrix3 kProd;
    for (int iRow = 0; iRow < 3; iRow++)
    {
        for (int iCol = 0; iCol < 3; iCol++)
        {
            int i = I(iRow,iCol);
            kProd.m_afEntry[i] = (Real)0.0;
            for (int iMid = 0; iMid < 3; iMid++)
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
Matrix3<Real> Matrix3<Real>::Inverse () const
{
    // Invert a 3x3 using cofactors.  This is faster than using a generic
    // Gaussian elimination because of the loop overhead of such a method.

    Matrix3 kInverse;

    kInverse[0][0] = m_afEntry[4]*m_afEntry[8] - m_afEntry[5]*m_afEntry[7];
    kInverse[0][1] = m_afEntry[2]*m_afEntry[7] - m_afEntry[1]*m_afEntry[8];
    kInverse[0][2] = m_afEntry[1]*m_afEntry[5] - m_afEntry[2]*m_afEntry[4];
    kInverse[1][0] = m_afEntry[5]*m_afEntry[6] - m_afEntry[3]*m_afEntry[8];
    kInverse[1][1] = m_afEntry[0]*m_afEntry[8] - m_afEntry[2]*m_afEntry[6];
    kInverse[1][2] = m_afEntry[2]*m_afEntry[3] - m_afEntry[0]*m_afEntry[5];
    kInverse[2][0] = m_afEntry[3]*m_afEntry[7] - m_afEntry[4]*m_afEntry[6];
    kInverse[2][1] = m_afEntry[1]*m_afEntry[6] - m_afEntry[0]*m_afEntry[7];
    kInverse[2][2] = m_afEntry[0]*m_afEntry[4] - m_afEntry[1]*m_afEntry[3];

    Real fDet = m_afEntry[0]*kInverse[0][0] + m_afEntry[1]*kInverse[1][0]+
        m_afEntry[2]*kInverse[2][0];

    if ( Math<Real>::FAbs(fDet) <= Math<Real>::ZERO_TOLERANCE )
        return ZERO;

    kInverse /= fDet;
    return kInverse;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::Adjoint () const
{
    Matrix3 kAdjoint;

    kAdjoint[0][0] = m_afEntry[4]*m_afEntry[8] - m_afEntry[5]*m_afEntry[7];
    kAdjoint[0][1] = m_afEntry[2]*m_afEntry[7] - m_afEntry[1]*m_afEntry[8];
    kAdjoint[0][2] = m_afEntry[1]*m_afEntry[5] - m_afEntry[2]*m_afEntry[4];
    kAdjoint[1][0] = m_afEntry[5]*m_afEntry[6] - m_afEntry[3]*m_afEntry[8];
    kAdjoint[1][1] = m_afEntry[0]*m_afEntry[8] - m_afEntry[2]*m_afEntry[6];
    kAdjoint[1][2] = m_afEntry[2]*m_afEntry[3] - m_afEntry[0]*m_afEntry[5];
    kAdjoint[2][0] = m_afEntry[3]*m_afEntry[7] - m_afEntry[4]*m_afEntry[6];
    kAdjoint[2][1] = m_afEntry[1]*m_afEntry[6] - m_afEntry[0]*m_afEntry[7];
    kAdjoint[2][2] = m_afEntry[0]*m_afEntry[4] - m_afEntry[1]*m_afEntry[3];

    return kAdjoint;
}
//----------------------------------------------------------------------------
template <class Real>
Real Matrix3<Real>::Determinant () const
{
    Real fCo00 = m_afEntry[4]*m_afEntry[8] - m_afEntry[5]*m_afEntry[7];
    Real fCo10 = m_afEntry[5]*m_afEntry[6] - m_afEntry[3]*m_afEntry[8];
    Real fCo20 = m_afEntry[3]*m_afEntry[7] - m_afEntry[4]*m_afEntry[6];
    Real fDet = m_afEntry[0]*fCo00 + m_afEntry[1]*fCo10 + m_afEntry[2]*fCo20;
    return fDet;
}
//----------------------------------------------------------------------------
template <class Real>
Real Matrix3<Real>::QForm (const Vector3<Real>& rkU, const Vector3<Real>& rkV) const
{
    return rkU.Dot((*this)*rkV);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::TimesDiagonal (const Vector3<Real>& rkDiag) const
{
    return Matrix3(
        m_afEntry[0]*rkDiag[0],m_afEntry[1]*rkDiag[1],m_afEntry[2]*rkDiag[2],
        m_afEntry[3]*rkDiag[0],m_afEntry[4]*rkDiag[1],m_afEntry[5]*rkDiag[2],
        m_afEntry[6]*rkDiag[0],m_afEntry[7]*rkDiag[1],m_afEntry[8]*rkDiag[2]);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::DiagonalTimes (const Vector3<Real>& rkDiag) const
{
    return Matrix3(
        rkDiag[0]*m_afEntry[0],rkDiag[0]*m_afEntry[1],rkDiag[0]*m_afEntry[2],
        rkDiag[1]*m_afEntry[3],rkDiag[1]*m_afEntry[4],rkDiag[1]*m_afEntry[5],
        rkDiag[2]*m_afEntry[6],rkDiag[2]*m_afEntry[7],rkDiag[2]*m_afEntry[8]);
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::ToAxisAngle (Vector3<Real>& rkAxis, Real& rfAngle) const
{
    // Let (x,y,z) be the unit-length axis and let A be an angle of rotation.
    // The rotation matrix is R = I + sin(A)*P + (1-cos(A))*P^2 where
    // I is the identity and
    //
    //       +-        -+
    //   P = |  0 -z +y |
    //       | +z  0 -x |
    //       | -y +x  0 |
    //       +-        -+
    //
    // If A > 0, R represents a counterclockwise rotation about the axis in
    // the sense of looking from the tip of the axis vector towards the
    // origin.  Some algebra will show that
    //
    //   cos(A) = (trace(R)-1)/2  and  R - R^t = 2*sin(A)*P
    //
    // In the event that A = pi, R-R^t = 0 which prevents us from extracting
    // the axis through P.  Instead note that R = I+2*P^2 when A = pi, so
    // P^2 = (R-I)/2.  The diagonal entries of P^2 are x^2-1, y^2-1, and
    // z^2-1.  We can solve these for axis (x,y,z).  Because the angle is pi,
    // it does not matter which sign you choose on the square roots.

    Real fTrace = m_afEntry[0] + m_afEntry[4] + m_afEntry[8];
    Real fCos = ((Real)0.5)*(fTrace-(Real)1.0);
    rfAngle = Math<Real>::ACos(fCos);  // in [0,PI]

    if ( rfAngle > (Real)0.0 )
    {
        if ( rfAngle < Math<Real>::PI )
        {
            rkAxis[0] = m_afEntry[7]-m_afEntry[5];
            rkAxis[1] = m_afEntry[2]-m_afEntry[6];
            rkAxis[2] = m_afEntry[3]-m_afEntry[1];
            rkAxis.Normalize();
        }
        else
        {
            // angle is PI
            Real fHalfInverse;
            if ( m_afEntry[0] >= m_afEntry[4] )
            {
                // r00 >= r11
                if ( m_afEntry[0] >= m_afEntry[8] )
                {
                    // r00 is maximum diagonal term
                    rkAxis[0] = ((Real)0.5)*Math<Real>::Sqrt(m_afEntry[0] -
                        m_afEntry[4] - m_afEntry[8] + (Real)1.0);
                    fHalfInverse = ((Real)0.5)/rkAxis[0];
                    rkAxis[1] = fHalfInverse*m_afEntry[1];
                    rkAxis[2] = fHalfInverse*m_afEntry[2];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis[2] = ((Real)0.5)*Math<Real>::Sqrt(m_afEntry[8] -
                        m_afEntry[0] - m_afEntry[4] + (Real)1.0);
                    fHalfInverse = ((Real)0.5)/rkAxis[2];
                    rkAxis[0] = fHalfInverse*m_afEntry[2];
                    rkAxis[1] = fHalfInverse*m_afEntry[5];
                }
            }
            else
            {
                // r11 > r00
                if ( m_afEntry[4] >= m_afEntry[8] )
                {
                    // r11 is maximum diagonal term
                    rkAxis[1] = ((Real)0.5)*Math<Real>::Sqrt(m_afEntry[4] -
                        m_afEntry[0] - m_afEntry[8] + (Real)1.0);
                    fHalfInverse  = ((Real)0.5)/rkAxis[1];
                    rkAxis[0] = fHalfInverse*m_afEntry[1];
                    rkAxis[2] = fHalfInverse*m_afEntry[5];
                }
                else
                {
                    // r22 is maximum diagonal term
                    rkAxis[2] = ((Real)0.5)*Math<Real>::Sqrt(m_afEntry[8] -
                        m_afEntry[0] - m_afEntry[4] + (Real)1.0);
                    fHalfInverse = ((Real)0.5)/rkAxis[2];
                    rkAxis[0] = fHalfInverse*m_afEntry[2];
                    rkAxis[1] = fHalfInverse*m_afEntry[5];
                }
            }
        }
    }
    else
    {
        // The angle is 0 and the matrix is the identity.  Any axis will
        // work, so just use the x-axis.
        rkAxis[0] = (Real)1.0;
        rkAxis[1] = (Real)0.0;
        rkAxis[2] = (Real)0.0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::Orthonormalize ()
{
    // Algorithm uses Gram-Schmidt orthogonalization.  If 'this' matrix is
    // M = [m0|m1|m2], then orthonormal output matrix is Q = [q0|q1|q2],
    //
    //   q0 = m0/|m0|
    //   q1 = (m1-(q0*m1)q0)/|m1-(q0*m1)q0|
    //   q2 = (m2-(q0*m2)q0-(q1*m2)q1)/|m2-(q0*m2)q0-(q1*m2)q1|
    //
    // where |V| indicates length of vector V and A*B indicates dot
    // product of vectors A and B.

    // compute q0
    Real fInvLength = Math<Real>::InvSqrt(m_afEntry[0]*m_afEntry[0] +
        m_afEntry[3]*m_afEntry[3] + m_afEntry[6]*m_afEntry[6]);

    m_afEntry[0] *= fInvLength;
    m_afEntry[3] *= fInvLength;
    m_afEntry[6] *= fInvLength;

    // compute q1
    Real fDot0 = m_afEntry[0]*m_afEntry[1] + m_afEntry[3]*m_afEntry[4] +
        m_afEntry[6]*m_afEntry[7];

    m_afEntry[1] -= fDot0*m_afEntry[0];
    m_afEntry[4] -= fDot0*m_afEntry[3];
    m_afEntry[7] -= fDot0*m_afEntry[6];

    fInvLength = Math<Real>::InvSqrt(m_afEntry[1]*m_afEntry[1] +
        m_afEntry[4]*m_afEntry[4] + m_afEntry[7]*m_afEntry[7]);

    m_afEntry[1] *= fInvLength;
    m_afEntry[4] *= fInvLength;
    m_afEntry[7] *= fInvLength;

    // compute q2
    Real fDot1 = m_afEntry[1]*m_afEntry[2] + m_afEntry[4]*m_afEntry[5] +
        m_afEntry[7]*m_afEntry[8];

    fDot0 = m_afEntry[0]*m_afEntry[2] + m_afEntry[3]*m_afEntry[5] +
        m_afEntry[6]*m_afEntry[8];

    m_afEntry[2] -= fDot0*m_afEntry[0] + fDot1*m_afEntry[1];
    m_afEntry[5] -= fDot0*m_afEntry[3] + fDot1*m_afEntry[4];
    m_afEntry[8] -= fDot0*m_afEntry[6] + fDot1*m_afEntry[7];

    fInvLength = Math<Real>::InvSqrt(m_afEntry[2]*m_afEntry[2] +
        m_afEntry[5]*m_afEntry[5] + m_afEntry[8]*m_afEntry[8]);

    m_afEntry[2] *= fInvLength;
    m_afEntry[5] *= fInvLength;
    m_afEntry[8] *= fInvLength;
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::EigenDecomposition (Matrix3& rkRot, Matrix3& rkDiag) const
{
    // Factor M = R*D*R^T.  The columns of R are the eigenvectors.  The
    // diagonal entries of D are the corresponding eigenvalues.
    Real afDiag[3], afSubDiag[3];
    rkRot = *this;
    rkRot.Tridiagonalize(afDiag,afSubDiag);
    rkRot.QLAlgorithm(afDiag,afSubDiag);

    // The Householder transformation is a reflection.  Make the eigenvectors
    // a right--handed system by changing sign on the last column.
    rkRot[2][0] = -rkRot[2][0];
    rkRot[2][1] = -rkRot[2][1];
    rkRot[2][2] = -rkRot[2][2];

    // (insertion) sort eigenvalues in increasing order, d0 <= d1 <= d2
    int i;
    Real fSave;

    if ( afDiag[1] < afDiag[0] )
    {
        // swap d0 and d1
        fSave = afDiag[0];
        afDiag[0] = afDiag[1];
        afDiag[1] = fSave;

        // swap V0 and V1
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][0];
            rkRot[i][0] = rkRot[i][1];
            rkRot[i][1] = fSave;
        }
    }

    if ( afDiag[2] < afDiag[1] )
    {
        // swap d1 and d2
        fSave = afDiag[1];
        afDiag[1] = afDiag[2];
        afDiag[2] = fSave;

        // swap V1 and V2
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][1];
            rkRot[i][1] = rkRot[i][2];
            rkRot[i][2] = fSave;
        }
    }

    if ( afDiag[1] < afDiag[0] )
    {
        // swap d0 and d1
        fSave = afDiag[0];
        afDiag[0] = afDiag[1];
        afDiag[1] = fSave;

        // swap V0 and V1
        for (i = 0; i < 3; i++)
        {
            fSave = rkRot[i][0];
            rkRot[i][0] = rkRot[i][1];
            rkRot[i][1] = fSave;
        }
    }

    rkDiag.MakeDiagonal(afDiag[0],afDiag[1],afDiag[2]);
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::FromEulerAnglesXYZ (Real fYAngle, Real fPAngle,
    Real fRAngle)
{
    Real fCos, fSin;

    fCos = Math<Real>::Cos(fYAngle);
    fSin = Math<Real>::Sin(fYAngle);
    Matrix3 kXMat(
        (Real)1.0,(Real)0.0,(Real)0.0,
        (Real)0.0,fCos,-fSin,
        (Real)0.0,fSin,fCos);

    fCos = Math<Real>::Cos(fPAngle);
    fSin = Math<Real>::Sin(fPAngle);
    Matrix3 kYMat(
        fCos,(Real)0.0,fSin,
        (Real)0.0,(Real)1.0,(Real)0.0,
        -fSin,(Real)0.0,fCos);

    fCos = Math<Real>::Cos(fRAngle);
    fSin = Math<Real>::Sin(fRAngle);
    Matrix3 kZMat(
        fCos,-fSin,(Real)0.0,
        fSin,fCos,(Real)0.0,
        (Real)0.0,(Real)0.0,(Real)1.0);

    *this = kXMat*(kYMat*kZMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::FromEulerAnglesXZY (Real fYAngle, Real fPAngle,
    Real fRAngle)
{
    Real fCos, fSin;

    fCos = Math<Real>::Cos(fYAngle);
    fSin = Math<Real>::Sin(fYAngle);
    Matrix3 kXMat(
        (Real)1.0,(Real)0.0,(Real)0.0,
        (Real)0.0,fCos,-fSin,
        (Real)0.0,fSin,fCos);

    fCos = Math<Real>::Cos(fPAngle);
    fSin = Math<Real>::Sin(fPAngle);
    Matrix3 kZMat(
        fCos,-fSin,(Real)0.0,
        fSin,fCos,(Real)0.0,
        (Real)0.0,(Real)0.0,(Real)1.0);

    fCos = Math<Real>::Cos(fRAngle);
    fSin = Math<Real>::Sin(fRAngle);
    Matrix3 kYMat(
        fCos,(Real)0.0,fSin,
        (Real)0.0,(Real)1.0,(Real)0.0,
        -fSin,(Real)0.0,fCos);

    *this = kXMat*(kZMat*kYMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::FromEulerAnglesYXZ (Real fYAngle, Real fPAngle,
    Real fRAngle)
{
    Real fCos, fSin;

    fCos = Math<Real>::Cos(fYAngle);
    fSin = Math<Real>::Sin(fYAngle);
    Matrix3 kYMat(
        fCos,(Real)0.0,fSin,
        (Real)0.0,(Real)1.0,(Real)0.0,
        -fSin,(Real)0.0,fCos);

    fCos = Math<Real>::Cos(fPAngle);
    fSin = Math<Real>::Sin(fPAngle);
    Matrix3 kXMat(
        (Real)1.0,(Real)0.0,(Real)0.0,
        (Real)0.0,fCos,-fSin,
        (Real)0.0,fSin,fCos);

    fCos = Math<Real>::Cos(fRAngle);
    fSin = Math<Real>::Sin(fRAngle);
    Matrix3 kZMat(
        fCos,-fSin,(Real)0.0,
        fSin,fCos,(Real)0.0,
        (Real)0.0,(Real)0.0,(Real)1.0);

    *this = kYMat*(kXMat*kZMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::FromEulerAnglesYZX (Real fYAngle, Real fPAngle,
    Real fRAngle)
{
    Real fCos, fSin;

    fCos = Math<Real>::Cos(fYAngle);
    fSin = Math<Real>::Sin(fYAngle);
    Matrix3 kYMat(
        fCos,(Real)0.0,fSin,
        (Real)0.0,(Real)1.0,(Real)0.0,
        -fSin,(Real)0.0,fCos);

    fCos = Math<Real>::Cos(fPAngle);
    fSin = Math<Real>::Sin(fPAngle);
    Matrix3 kZMat(
        fCos,-fSin,(Real)0.0,
        fSin,fCos,(Real)0.0,
        (Real)0.0,(Real)0.0,(Real)1.0);

    fCos = Math<Real>::Cos(fRAngle);
    fSin = Math<Real>::Sin(fRAngle);
    Matrix3 kXMat(
        (Real)1.0,(Real)0.0,(Real)0.0,
        (Real)0.0,fCos,-fSin,
        (Real)0.0,fSin,fCos);

    *this = kYMat*(kZMat*kXMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::FromEulerAnglesZXY (Real fYAngle, Real fPAngle,
    Real fRAngle)
{
    Real fCos, fSin;

    fCos = Math<Real>::Cos(fYAngle);
    fSin = Math<Real>::Sin(fYAngle);
    Matrix3 kZMat(
        fCos,-fSin,(Real)0.0,
        fSin,fCos,(Real)0.0,
        (Real)0.0,(Real)0.0,(Real)1.0);

    fCos = Math<Real>::Cos(fPAngle);
    fSin = Math<Real>::Sin(fPAngle);
    Matrix3 kXMat(
        (Real)1.0,(Real)0.0,(Real)0.0,
        (Real)0.0,fCos,-fSin,
        (Real)0.0,fSin,fCos);

    fCos = Math<Real>::Cos(fRAngle);
    fSin = Math<Real>::Sin(fRAngle);
    Matrix3 kYMat(
        fCos,(Real)0.0,fSin,
        (Real)0.0,(Real)1.0,(Real)0.0,
        -fSin,(Real)0.0,fCos);

    *this = kZMat*(kXMat*kYMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real>& Matrix3<Real>::FromEulerAnglesZYX (Real fYAngle, Real fPAngle,
    Real fRAngle)
{
    Real fCos, fSin;

    fCos = Math<Real>::Cos(fYAngle);
    fSin = Math<Real>::Sin(fYAngle);
    Matrix3 kZMat(
        fCos,-fSin,(Real)0.0,
        fSin,fCos,(Real)0.0,
        (Real)0.0,(Real)0.0,(Real)1.0);

    fCos = Math<Real>::Cos(fPAngle);
    fSin = Math<Real>::Sin(fPAngle);
    Matrix3 kYMat(
        fCos,(Real)0.0,fSin,
        (Real)0.0,(Real)1.0,(Real)0.0,
        -fSin,(Real)0.0,fCos);

    fCos = Math<Real>::Cos(fRAngle);
    fSin = Math<Real>::Sin(fRAngle);
    Matrix3 kXMat(
        (Real)1.0,(Real)0.0,(Real)0.0,
        (Real)0.0,fCos,-fSin,
        (Real)0.0,fSin,fCos);

    *this = kZMat*(kYMat*kXMat);
    return *this;
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::ToEulerAnglesXYZ (Real& rfXAngle, Real& rfYAngle,
    Real& rfZAngle) const
{
    // rot =  cy*cz          -cy*sz           sy
    //        cz*sx*sy+cx*sz  cx*cz-sx*sy*sz -cy*sx
    //       -cx*cz*sy+sx*sz  cz*sx+cx*sy*sz  cx*cy

    if ( m_afEntry[2] < (Real)1.0 )
    {
        if ( m_afEntry[2] > -(Real)1.0 )
        {
            rfXAngle = Math<Real>::ATan2(-m_afEntry[5],m_afEntry[8]);
            rfYAngle = (Real)asin((double)m_afEntry[2]);
            rfZAngle = Math<Real>::ATan2(-m_afEntry[1],m_afEntry[0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - ZA = -atan2(r10,r11)
            rfXAngle = -Math<Real>::ATan2(m_afEntry[3],m_afEntry[4]);
            rfYAngle = -Math<Real>::HALF_PI;
            rfZAngle = (Real)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XAngle + ZAngle = atan2(r10,r11)
        rfXAngle = Math<Real>::ATan2(m_afEntry[3],m_afEntry[4]);
        rfYAngle = Math<Real>::HALF_PI;
        rfZAngle = (Real)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::ToEulerAnglesXZY (Real& rfXAngle, Real& rfZAngle,
    Real& rfYAngle) const
{
    // rot =  cy*cz          -sz              cz*sy
    //        sx*sy+cx*cy*sz  cx*cz          -cy*sx+cx*sy*sz
    //       -cx*sy+cy*sx*sz  cz*sx           cx*cy+sx*sy*sz

    if ( m_afEntry[1] < (Real)1.0 )
    {
        if ( m_afEntry[1] > -(Real)1.0 )
        {
            rfXAngle = Math<Real>::ATan2(m_afEntry[7],m_afEntry[4]);
            rfZAngle = (Real)asin(-(double)m_afEntry[1]);
            rfYAngle = Math<Real>::ATan2(m_afEntry[2],m_afEntry[0]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  XA - YA = atan2(r20,r22)
            rfXAngle = Math<Real>::ATan2(m_afEntry[6],m_afEntry[8]);
            rfZAngle = Math<Real>::HALF_PI;
            rfYAngle = (Real)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  XA + YA = atan2(-r20,r22)
        rfXAngle = Math<Real>::ATan2(-m_afEntry[6],m_afEntry[8]);
        rfZAngle = -Math<Real>::HALF_PI;
        rfYAngle = (Real)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::ToEulerAnglesYXZ (Real& rfYAngle, Real& rfXAngle,
    Real& rfZAngle) const
{
    // rot =  cy*cz+sx*sy*sz  cz*sx*sy-cy*sz  cx*sy
    //        cx*sz           cx*cz          -sx
    //       -cz*sy+cy*sx*sz  cy*cz*sx+sy*sz  cx*cy

    if ( m_afEntry[5] < (Real)1.0 )
    {
        if ( m_afEntry[5] > -(Real)1.0 )
        {
            rfYAngle = Math<Real>::ATan2(m_afEntry[2],m_afEntry[8]);
            rfXAngle = (Real)asin(-(double)m_afEntry[5]);
            rfZAngle = Math<Real>::ATan2(m_afEntry[3],m_afEntry[4]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - ZA = atan2(r01,r00)
            rfYAngle = Math<Real>::ATan2(m_afEntry[1],m_afEntry[0]);
            rfXAngle = Math<Real>::HALF_PI;
            rfZAngle = (Real)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + ZA = atan2(-r01,r00)
        rfYAngle = Math<Real>::ATan2(-m_afEntry[1],m_afEntry[0]);
        rfXAngle = -Math<Real>::HALF_PI;
        rfZAngle = (Real)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::ToEulerAnglesYZX (Real& rfYAngle, Real& rfZAngle,
    Real& rfXAngle) const
{
    // rot =  cy*cz           sx*sy-cx*cy*sz  cx*sy+cy*sx*sz
    //        sz              cx*cz          -cz*sx
    //       -cz*sy           cy*sx+cx*sy*sz  cx*cy-sx*sy*sz

    if ( m_afEntry[3] < (Real)1.0 )
    {
        if ( m_afEntry[3] > -(Real)1.0 )
        {
            rfYAngle = Math<Real>::ATan2(-m_afEntry[6],m_afEntry[0]);
            rfZAngle = (Real)asin((double)m_afEntry[3]);
            rfXAngle = Math<Real>::ATan2(-m_afEntry[5],m_afEntry[4]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  YA - XA = -atan2(r21,r22);
            rfYAngle = -Math<Real>::ATan2(m_afEntry[7],m_afEntry[8]);
            rfZAngle = -Math<Real>::HALF_PI;
            rfXAngle = (Real)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  YA + XA = atan2(r21,r22)
        rfYAngle = Math<Real>::ATan2(m_afEntry[7],m_afEntry[8]);
        rfZAngle = Math<Real>::HALF_PI;
        rfXAngle = (Real)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::ToEulerAnglesZXY (Real& rfZAngle, Real& rfXAngle,
    Real& rfYAngle) const
{
    // rot =  cy*cz-sx*sy*sz -cx*sz           cz*sy+cy*sx*sz
    //        cz*sx*sy+cy*sz  cx*cz          -cy*cz*sx+sy*sz
    //       -cx*sy           sx              cx*cy

    if ( m_afEntry[7] < (Real)1.0 )
    {
        if ( m_afEntry[7] > -(Real)1.0 )
        {
            rfZAngle = Math<Real>::ATan2(-m_afEntry[1],m_afEntry[4]);
            rfXAngle = (Real)asin((double)m_afEntry[7]);
            rfYAngle = Math<Real>::ATan2(-m_afEntry[6],m_afEntry[8]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - YA = -atan(r02,r00)
            rfZAngle = -Math<Real>::ATan2(m_afEntry[2],m_afEntry[0]);
            rfXAngle = -Math<Real>::HALF_PI;
            rfYAngle = (Real)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + YA = atan2(r02,r00)
        rfZAngle = Math<Real>::ATan2(m_afEntry[2],m_afEntry[0]);
        rfXAngle = Math<Real>::HALF_PI;
        rfYAngle = (Real)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::ToEulerAnglesZYX (Real& rfZAngle, Real& rfYAngle,
    Real& rfXAngle) const
{
    // rot =  cy*cz           cz*sx*sy-cx*sz  cx*cz*sy+sx*sz
    //        cy*sz           cx*cz+sx*sy*sz -cz*sx+cx*sy*sz
    //       -sy              cy*sx           cx*cy

    if ( m_afEntry[6] < (Real)1.0 )
    {
        if ( m_afEntry[6] > -(Real)1.0 )
        {
            rfZAngle = Math<Real>::ATan2(m_afEntry[3],m_afEntry[0]);
            rfYAngle = (Real)asin(-(double)m_afEntry[6]);
            rfXAngle = Math<Real>::ATan2(m_afEntry[7],m_afEntry[8]);
            return true;
        }
        else
        {
            // WARNING.  Not unique.  ZA - XA = -atan2(r01,r02)
            rfZAngle = -Math<Real>::ATan2(m_afEntry[1],m_afEntry[2]);
            rfYAngle = Math<Real>::HALF_PI;
            rfXAngle = (Real)0.0;
            return false;
        }
    }
    else
    {
        // WARNING.  Not unique.  ZA + XA = atan2(-r01,-r02)
        rfZAngle = Math<Real>::ATan2(-m_afEntry[1],-m_afEntry[2]);
        rfYAngle = -Math<Real>::HALF_PI;
        rfXAngle = (Real)0.0;
        return false;
    }
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> Matrix3<Real>::Slerp (Real fT, const Matrix3& rkR0,
    const Matrix3& rkR1)
{
    Vector3<Real> kAxis;
    Real fAngle;
    Matrix3 kProd = rkR0.TransposeTimes(rkR1);
    kProd.ToAxisAngle(kAxis,fAngle);
    return Matrix3(kAxis,fT*fAngle);
}
//----------------------------------------------------------------------------
template <class Real>
void Matrix3<Real>::Tridiagonalize (Real afDiag[3], Real afSubDiag[3])
{
    // Householder reduction T = Q^t M Q
    //   Input:   
    //     mat, symmetric 3x3 matrix M
    //   Output:  
    //     mat, orthogonal matrix Q (a reflection)
    //     diag, diagonal entries of T
    //     subd, subdiagonal entries of T (T is symmetric)

    Real fA = m_afEntry[0];
    Real fB = m_afEntry[1];
    Real fC = m_afEntry[2];
    Real fD = m_afEntry[4];
    Real fE = m_afEntry[5];
    Real fF = m_afEntry[8];

    afDiag[0] = fA;
    afSubDiag[2] = (Real)0.0;
    if ( Math<Real>::FAbs(fC) >= Math<Real>::ZERO_TOLERANCE )
    {
        Real fLength = Math<Real>::Sqrt(fB*fB+fC*fC);
        Real fInvLength = ((Real)1.0)/fLength;
        fB *= fInvLength;
        fC *= fInvLength;
        Real fQ = ((Real)2.0)*fB*fE+fC*(fF-fD);
        afDiag[1] = fD+fC*fQ;
        afDiag[2] = fF-fC*fQ;
        afSubDiag[0] = fLength;
        afSubDiag[1] = fE-fB*fQ;

        m_afEntry[0] = (Real)1.0;
        m_afEntry[1] = (Real)0.0;
        m_afEntry[2] = (Real)0.0;
        m_afEntry[3] = (Real)0.0;
        m_afEntry[4] = fB;
        m_afEntry[5] = fC;
        m_afEntry[6] = (Real)0.0;
        m_afEntry[7] = fC;
        m_afEntry[8] = -fB;
    }
    else
    {
        afDiag[1] = fD;
        afDiag[2] = fF;
        afSubDiag[0] = fB;
        afSubDiag[1] = fE;

        m_afEntry[0] = (Real)1.0;
        m_afEntry[1] = (Real)0.0;
        m_afEntry[2] = (Real)0.0;
        m_afEntry[3] = (Real)0.0;
        m_afEntry[4] = (Real)1.0;
        m_afEntry[5] = (Real)0.0;
        m_afEntry[6] = (Real)0.0;
        m_afEntry[7] = (Real)0.0;
        m_afEntry[8] = -(Real)1.0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Matrix3<Real>::QLAlgorithm (Real afDiag[3], Real afSubDiag[3])
{
    // QL iteration with implicit shifting to reduce matrix from tridiagonal
    // to diagonal

    for (int i0 = 0; i0 < 3; i0++)
    {
        const int iMaxIter = 32;
        int iIter;
        for (iIter = 0; iIter < iMaxIter; iIter++)
        {
            int i1;
            for (i1 = i0; i1 <= 1; i1++)
            {
                Real fSum = Math<Real>::FAbs(afDiag[i1]) +
                    Math<Real>::FAbs(afDiag[i1+1]);
                if ( Math<Real>::FAbs(afSubDiag[i1]) + fSum == fSum )
                    break;
            }
            if ( i1 == i0 )
                break;

            Real fTmp0 = (afDiag[i0+1] - afDiag[i0]) /
                (((Real)2.0)*afSubDiag[i0]);
            Real fTmp1 = Math<Real>::Sqrt(fTmp0*fTmp0+1.0f);
            if ( fTmp0 < (Real)0.0 )
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0-fTmp1);
            else
                fTmp0 = afDiag[i1]-afDiag[i0]+afSubDiag[i0]/(fTmp0+fTmp1);
            Real fSin = (Real)1.0;
            Real fCos = (Real)1.0;
            Real fTmp2 = (Real)0.0;
            for (int i2 = i1-1; i2 >= i0; i2--)
            {
                Real fTmp3 = fSin*afSubDiag[i2];
                Real fTmp4 = fCos*afSubDiag[i2];
                if ( Math<Real>::FAbs(fTmp3) >= Math<Real>::FAbs(fTmp0) )
                {
                    fCos = fTmp0/fTmp3;
                    fTmp1 = Math<Real>::Sqrt(fCos*fCos+(Real)1.0);
                    afSubDiag[i2+1] = fTmp3*fTmp1;
                    fSin = ((Real)1.0)/fTmp1;
                    fCos *= fSin;
                }
                else
                {
                    fSin = fTmp3/fTmp0;
                    fTmp1 = Math<Real>::Sqrt(fSin*fSin+(Real)1.0);
                    afSubDiag[i2+1] = fTmp0*fTmp1;
                    fCos = ((Real)1.0)/fTmp1;
                    fSin *= fCos;
                }
                fTmp0 = afDiag[i2+1]-fTmp2;
                fTmp1 = (afDiag[i2]-fTmp0)*fSin+((Real)2.0)*fTmp4*fCos;
                fTmp2 = fSin*fTmp1;
                afDiag[i2+1] = fTmp0+fTmp2;
                fTmp0 = fCos*fTmp1-fTmp4;

                for (int iRow = 0; iRow < 3; iRow++)
                {
                    fTmp3 = m_afEntry[I(iRow,i2+1)];
                    m_afEntry[I(iRow,i2+1)] = fSin*m_afEntry[I(iRow,i2)] +
                        fCos*fTmp3;
                    m_afEntry[I(iRow,i2)] = fCos*m_afEntry[I(iRow,i2)] -
                        fSin*fTmp3;
                }
            }
            afDiag[i0] -= fTmp2;
            afSubDiag[i0] = fTmp0;
            afSubDiag[i1] = (Real)0.0;
        }

        if ( iIter == iMaxIter )
        {
            // should not get here under normal circumstances
            assert( false );
            return false;
        }
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
Matrix3<Real> operator* (Real fScalar, const Matrix3<Real>& rkM)
{
    return rkM*fScalar;
}
//----------------------------------------------------------------------------
template <class Real>
Vector3<Real> operator* (const Vector3<Real>& rkV, const Matrix3<Real>& rkM)
{
    return Vector3<Real>(
        rkV[0]*rkM[0][0] + rkV[1]*rkM[1][0] + rkV[2]*rkM[2][0],
        rkV[0]*rkM[0][1] + rkV[1]*rkM[1][1] + rkV[2]*rkM[2][1],
        rkV[0]*rkM[0][2] + rkV[1]*rkM[1][2] + rkV[2]*rkM[2][2]);
}
//----------------------------------------------------------------------------
