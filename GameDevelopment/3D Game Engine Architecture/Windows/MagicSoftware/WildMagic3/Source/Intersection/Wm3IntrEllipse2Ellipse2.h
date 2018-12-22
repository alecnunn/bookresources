// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRELLIPSE2ELLIPSE2_H
#define WM3INTRELLIPSE2ELLIPSE2_H

#include "Wm3Intersector.h"
#include "Wm3Ellipse2.h"
#include "Wm3Polynomial1.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrEllipse2Ellipse2 : public Intersector<Real,Vector2<Real> >
{
public:
    IntrEllipse2Ellipse2 (const Ellipse2<Real>& rkEllipse0,
        const Ellipse2<Real>& rkEllipse1);

    // object access
    const Ellipse2<Real>& GetEllipse0 () const;
    const Ellipse2<Real>& GetEllipse1 () const;

    // static intersection queries
    virtual bool Test ();
    virtual bool Find ();

    // Intersection set for static find-intersection query.  The quantity Q
    // satisfies 0 <= Q <= 4.  When Q > 0, the interpretation depends on the
    // intersection type.
    //   IT_POINT:  Q distinct points of intersection
    //   IT_OTHER:  The ellipses are the same.  One of the ellipse objects is
    //              returned by GetIntersectionEllipse.  Q is invalid.
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;
    const Ellipse2<Real>& GetIntersectionEllipse () const;

private:
    static void GetAABB (const Ellipse2<Real>& rkEllipse, Real& rfXMin,
        Real& rfXMax, Real& rfYMin, Real& rfYMax);

    static Polynomial1<Real> GetQuartic (const Ellipse2<Real>& rkEllipse0,
        const Ellipse2<Real>& rkEllipse1);

    void ScaleEllipses (Ellipse2<Real>& rkScaled0, Ellipse2<Real>& rkScaled1,
        Real& rfMin, Real& rfHalfRange);

    Real EvaluateQuadratic (const Real afQP[6], Real fX, Real fY);

    // the objects to intersect
    const Ellipse2<Real>& m_rkEllipse0;
    const Ellipse2<Real>& m_rkEllipse1;

    // points of intersection
    int m_iQuantity;
    Vector2<Real> m_akPoint[4];
};

typedef IntrEllipse2Ellipse2<float> IntrEllipse2Ellipse2f;
typedef IntrEllipse2Ellipse2<double> IntrEllipse2Ellipse2d;

}

#endif
