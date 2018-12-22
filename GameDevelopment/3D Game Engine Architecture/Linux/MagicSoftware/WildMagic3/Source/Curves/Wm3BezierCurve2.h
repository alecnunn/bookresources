// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3BEZIERCURVE2_H
#define WM3BEZIERCURVE2_H

#include "Wm3SingleCurve2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM BezierCurve2 : public SingleCurve2<Real>
{
public:
    // Construction and destruction.  BezierCurve2 accepts responsibility for
    // deleting the input array.
    BezierCurve2 (int iDegree, Vector2<Real>* akCtrlPoint);
    virtual ~BezierCurve2 ();

    int GetDegree () const;
    const Vector2<Real>* GetControlPoints () const;

    virtual Vector2<Real> GetPosition (Real fTime) const;
    virtual Vector2<Real> GetFirstDerivative (Real fTime) const;
    virtual Vector2<Real> GetSecondDerivative (Real fTime) const;
    virtual Vector2<Real> GetThirdDerivative (Real fTime) const;

    virtual Real GetVariation (Real fT0, Real fT1,
        const Vector2<Real>* pkP0 = 0, const Vector2<Real>* pkP1 = 0) const;

protected:
    int m_iDegree;
    int m_iNumCtrlPoints;
    Vector2<Real>* m_akCtrlPoint;
    Vector2<Real>* m_akDer1CtrlPoint;
    Vector2<Real>* m_akDer2CtrlPoint;
    Vector2<Real>* m_akDer3CtrlPoint;
    Real** m_aafChoose;

    // variation support
    int m_iTwoDegree;
    int m_iTwoDegreePlusOne;
    Real* m_afSigma;
    Real* m_afRecip;
    Real* m_afPowT0;
    Real* m_afPowOmT0;
    Real* m_afPowT1;
    Real* m_afPowOmT1;
};

typedef BezierCurve2<float> BezierCurve2f;
typedef BezierCurve2<double> BezierCurve2d;

}

#endif
