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
Ellipse2<Real>::Ellipse2 ()
{
    // uninitialized
}
//----------------------------------------------------------------------------
template <class Real>
Ellipse2<Real>::Ellipse2 (const Vector2<Real>& rkCenter,
    const Vector2<Real>* akAxis, const Real* afExtent)
    :
    Center(rkCenter)
{
    for (int i = 0; i < 2; i++)
    {
        Axis[i] = akAxis[i];
        Extent[i] = afExtent[i];
    }
}
//----------------------------------------------------------------------------
template <class Real>
Ellipse2<Real>::Ellipse2 (const Vector2<Real>& rkCenter,
    const Vector2<Real>& rkAxis0, const Vector2<Real>& rkAxis1, Real fExtent0,
    Real fExtent1)
    :
    Center(rkCenter)
{
    Axis[0] = rkAxis0;
    Axis[1] = rkAxis1;
    Extent[0] = fExtent0;
    Extent[1] = fExtent1;
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipse2<Real>::GetM (Matrix2<Real>& rkM) const
{
    Vector2<Real> kRatio0 = Axis[0]/Extent[0];
    Vector2<Real> kRatio1 = Axis[1]/Extent[1];
    rkM = Matrix2<Real>(kRatio0,kRatio0) + Matrix2<Real>(kRatio1,kRatio1);
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipse2<Real>::GetMInverse (Matrix2<Real>& rkMInverse) const
{
    Vector2<Real> kRatio0 = Axis[0]*Extent[0];
    Vector2<Real> kRatio1 = Axis[1]*Extent[1];
    rkMInverse = Matrix2<Real>(kRatio0,kRatio0) +
        Matrix2<Real>(kRatio1,kRatio1);
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipse2<Real>::ToCoefficients (Real afCoeff[6]) const
{
    Matrix2<Real> kA;
    Vector2<Real> kB;
    Real fC;
    ToCoefficients(kA,kB,fC);
    Convert(kA,kB,fC,afCoeff);

    // arrange for one of the x0^2 or x1^2 coefficients to be 1
    Real fMax = Math<Real>::FAbs(afCoeff[3]);
    int iMax = 3;
    Real fAbs = Math<Real>::FAbs(afCoeff[5]);
    if ( fAbs > fMax )
    {
        fMax = fAbs;
        iMax = 5;
    }

    Real fInvMax = ((Real)1.0)/fMax;
    for (int i = 0; i < 6; i++)
    {
        if ( i != iMax )
            afCoeff[i] *= fInvMax;
        else
            afCoeff[i] = (Real)1.0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipse2<Real>::ToCoefficients (Matrix2<Real>& rkA, Vector2<Real>& rkB,
    Real& rfC) const
{
    Vector2<Real> kRatio0 = Axis[0]/Extent[0];
    Vector2<Real> kRatio1 = Axis[1]/Extent[1];
    rkA = Matrix2<Real>(kRatio0,kRatio0) + Matrix2<Real>(kRatio1,kRatio1);
    rkB = ((Real)-2.0)*(rkA*Center);
    rfC = rkA.QForm(Center,Center) - (Real)1.0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Ellipse2<Real>::FromCoefficients (const Real afCoeff[6])
{
    Matrix2<Real> kA;
    Vector2<Real> kB;
    Real fC;
    Convert(afCoeff,kA,kB,fC);
    return FromCoefficients(kA,kB,fC);
}
//----------------------------------------------------------------------------
template <class Real>
bool Ellipse2<Real>::FromCoefficients (const Matrix2<Real>& rkA,
    const Vector2<Real>& rkB, Real fC)
{
    // compute the center K = -A^{-1}*B/2
    Matrix2<Real> kInvA = rkA.Inverse();
    if ( kInvA == Matrix2<Real>::ZERO )
        return false;

    Center = ((Real)-0.5)*(kInvA*rkB);

    // compute B^T*A^{-1}*B/4 - C = K^T*A*K - C = -K^T*B/2 - C
    Real fRHS = -((Real)0.5)*(Center.Dot(rkB)) - fC;
    if ( Math<Real>::FAbs(fRHS) < Math<Real>::ZERO_TOLERANCE )
        return false;

    // compute M = A/(K^T*A*K - C)
    Real fInvRHS = ((Real)1.0)/fRHS;
    Matrix2<Real> kM = fInvRHS*rkA;

    // factor into M = R*D*R^T
    Eigen<Real> kES(kM);
    kES.IncrSortEigenStuff2();
    for (int i = 0; i < 2; i++)
    {
        if ( kES.GetEigenvalue(i) <= (Real)0.0 )
            return false;

        Extent[i] = Math<Real>::InvSqrt(kES.GetEigenvalue(i));
        kES.GetEigenvector(i,Axis[i]);
    }

    return true;
}
//----------------------------------------------------------------------------
template <class Real>
bool Ellipse2<Real>::Contains (const Vector2<Real>& rkPoint)
{
    Vector2<Real> kDiff = rkPoint - Center;
    Real fRatio0 = Axis[0].Dot(kDiff)/Extent[0];
    Real fRatio1 = Axis[1].Dot(kDiff)/Extent[1];
    Real fTest = fRatio0*fRatio0 + fRatio1*fRatio1 - (Real)1.0;
    return fTest <= (Real)0.0;
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipse2<Real>::Convert (const Real afCoeff[6], Matrix2<Real>& rkA,
    Vector2<Real>& rkB, Real& rfC)
{
    rfC = afCoeff[0];
    rkB[0] = afCoeff[1];
    rkB[1] = afCoeff[2];
    rkB[2] = afCoeff[3];
    rkA[0][0] = afCoeff[4];
    rkA[0][1] = ((Real)0.5)*afCoeff[5];
    rkA[0][2] = ((Real)0.5)*afCoeff[6];
    rkA[1][0] = rkA[0][1];
    rkA[1][1] = afCoeff[7];
    rkA[1][2] = ((Real)0.5)*afCoeff[8];
    rkA[2][0] = rkA[0][2];
    rkA[2][1] = rkA[1][2];
    rkA[2][2] = afCoeff[9];
}
//----------------------------------------------------------------------------
template <class Real>
void Ellipse2<Real>::Convert (const Matrix2<Real>& rkA,
    const Vector2<Real>& rkB, Real fC, Real afCoeff[6])
{
    afCoeff[0] = fC;
    afCoeff[1] = rkB[0];
    afCoeff[2] = rkB[1];
    afCoeff[3] = rkB[2];
    afCoeff[4] = rkA[0][0];
    afCoeff[5] = ((Real)2.0)*rkA[0][1];
    afCoeff[6] = ((Real)2.0)*rkA[0][2];
    afCoeff[7] = rkA[1][1];
    afCoeff[8] = ((Real)2.0)*rkA[1][2];
    afCoeff[9] = rkA[2][2];
}
//----------------------------------------------------------------------------
