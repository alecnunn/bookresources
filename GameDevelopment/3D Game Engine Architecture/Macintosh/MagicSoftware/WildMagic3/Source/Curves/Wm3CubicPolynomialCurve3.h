// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3CUBICPOLYNOMIALCURVE3_H
#define WM3CUBICPOLYNOMIALCURVE3_H

#include "Wm3PolynomialCurve3.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM CubicPolynomialCurve3 : public PolynomialCurve3<Real>
{
public:
    // Construction and destruction.  CubicPolynomialCurve3 accepts
    // responsibility for deleting the input polynomials.
    CubicPolynomialCurve3 (Polynomial1<Real>* pkXPoly,
        Polynomial1<Real>* pkYPoly, Polynomial1<Real>* pkZPoly);

    virtual ~CubicPolynomialCurve3 ();

    // tessellation data
    int GetVertexQuantity () const;
    Vector3<Real>* Vertices ();

    // tessellation by recursive subdivision
    void Tessellate (int iLevel);

protected:
    // precomputation
    class WM3_ITEM IntervalParameters
    {
    public:
        int m_iI0, m_iI1;
        Vector3<Real> m_akXuu[2];
    };

    // subdivide curve into two halves
    void Subdivide (int iLevel, Real fDSqr, Vector3<Real>* akX,
        IntervalParameters& rkIP);

    // tessellation data
    int m_iVertexQuantity;
    Vector3<Real>* m_akVertex;
};

typedef CubicPolynomialCurve3<float> CubicPolynomialCurve3f;
typedef CubicPolynomialCurve3<double> CubicPolynomialCurve3d;

}

#endif
