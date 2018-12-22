// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WM3INTRTRIANGLE2TRIANGLE2_H
#define WM3INTRTRIANGLE2TRIANGLE2_H

#include "Wm3Intersector.h"
#include "Wm3Intersector1.h"
#include "Wm3Triangle2.h"

namespace Wm3
{

template <class Real>
class WM3_ITEM IntrTriangle2Triangle2 :
    public Intersector<Real,Vector2<Real> >
{
public:
    IntrTriangle2Triangle2 (const Triangle2<Real>& rkTriangle0,
        const Triangle2<Real>& rkTriangle1);

    // object access
    const Triangle2<Real>& GetTriangle0 () const;
    const Triangle2<Real>& GetTriangle1 () const;

    // static queries
    virtual bool Test ();
    virtual bool Find ();

    // dynamic queries
    virtual bool Test (Real fTMax, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);
    virtual bool Find (Real fTMax, const Vector2<Real>& rkVelocity0,
        const Vector2<Real>& rkVelocity1);

    // information about the intersection set
    int GetQuantity () const;
    const Vector2<Real>& GetPoint (int i) const;

private:
    static int WhichSide (const Vector2<Real> akV[3],
        const Vector2<Real>& rkP, const Vector2<Real>& rkD);

    static void ClipConvexPolygonAgainstLine (const Vector2<Real>& rkN,
        Real fC, int& riQuantity, Vector2<Real> akV[6]);

    class Configuration
    {
    public:
        void ComputeTwo (const Vector2<Real> akV[3], const Vector2<Real>& rkD,
            int iI0, int iI1, int iI2);

        void ComputeThree (const Vector2<Real> akV[3],
            const Vector2<Real>& rkD, const Vector2<Real>& rkP);

        static bool NoIntersect (const Configuration& rkCfg0,
            const Configuration& rkCfg1, Real fTMax, Real fSpeed, int& riSide,
            Configuration& rkTCfg0, Configuration& rkTCfg1, Real& rfTFirst,
            Real& rfTLast);

        static void GetIntersection (const Configuration& rkCfg0,
            const Configuration& rkCfg1, int iSide,
            const Vector2<Real> akV0[3], const Vector2<Real> akV1[3],
            int& riQuantity, Vector2<Real> akVertex[6]);

    private:
        enum ProjectionMap
        {
            M21,  // 2 vertices map to min, 1 vertex maps to max
            M12,  // 1 vertex maps to min, 2 vertices map to max
            M11   // 1 vertex maps to min, 1 vertex maps to max
        };

        ProjectionMap m_eMap;  // how vertices map to the projection interval
        int m_aiIndex[3];      // the sorted indices of the vertices
        Real m_fMin, m_fMax;   // the interval is [min,max]
    };

    // the objects to intersect
    const Triangle2<Real>& m_rkTriangle0;
    const Triangle2<Real>& m_rkTriangle1;

    // information about the intersection set
    int m_iQuantity;
    Vector2<Real> m_akPoint[6];
};

typedef IntrTriangle2Triangle2<float> IntrTriangle2Triangle2f;
typedef IntrTriangle2Triangle2<double> IntrTriangle2Triangle2d;

}

#endif
