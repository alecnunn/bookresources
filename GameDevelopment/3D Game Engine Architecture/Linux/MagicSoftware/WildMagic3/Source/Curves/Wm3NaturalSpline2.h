// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3NATURALSPLINE2_H
#define WM3NATURALSPLINE2_H

#include "Wm3MultipleCurve2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM NaturalSpline2 : public MultipleCurve2<Real>
{
public:
    enum BoundaryType
    {
        BT_FREE,
        BT_CLAMPED,
        BT_CLOSED
    };

    // Construction and destruction.  NaturalSpline2 accepts responsibility
    // for deleting the input arrays.
    NaturalSpline2 (BoundaryType eType, int iSegments, Real* afTime,
        Vector2<Real>* akPoint);

    virtual ~NaturalSpline2 ();

    const Vector2<Real>* GetPoints () const;

    virtual Vector2<Real> GetPosition (Real fTime) const;
    virtual Vector2<Real> GetFirstDerivative (Real fTime) const;
    virtual Vector2<Real> GetSecondDerivative (Real fTime) const;
    virtual Vector2<Real> GetThirdDerivative (Real fTime) const;

protected:
    void CreateFreeSpline ();
    void CreateClampedSpline ();
    void CreateClosedSpline ();

    virtual Real GetSpeedKey (int iKey, Real fTime) const;
    virtual Real GetLengthKey (int iKey, Real fT0, Real fT1) const;
    virtual Real GetVariationKey (int iKey, Real fT0, Real fT1,
        const Vector2<Real>& rkA, const Vector2<Real>& rkB) const;

    Vector2<Real>* m_akA;
    Vector2<Real>* m_akB;
    Vector2<Real>* m_akC;
    Vector2<Real>* m_akD;

    class ThisPlusKey
    {
    public:
        ThisPlusKey (const NaturalSpline2* pkThis, int iKey)
            :
            This(pkThis),
            Key(iKey)
        {
        }

        const NaturalSpline2* This;
        int Key;
    };
};

typedef NaturalSpline2<float> NaturalSpline2f;
typedef NaturalSpline2<double> NaturalSpline2d;

}

#endif
