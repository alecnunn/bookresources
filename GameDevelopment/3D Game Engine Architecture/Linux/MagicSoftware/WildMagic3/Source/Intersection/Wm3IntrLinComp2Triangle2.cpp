// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrLinComp2Triangle2.h"
#include "Wm3Intersector1.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrLinComp2Triangle2<Real>::IntrLinComp2Triangle2 (
    const LinComp2<Real>& rkComponent, const Triangle2<Real>& rkTriangle)
    :
    m_rkComponent(rkComponent),
    m_rkTriangle(rkTriangle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const LinComp2<Real>& IntrLinComp2Triangle2<Real>::GetComponent ()
    const
{
    return m_rkComponent;
}
//----------------------------------------------------------------------------
template <class Real>
const Triangle2<Real>&
IntrLinComp2Triangle2<Real>::GetTriangle () const
{
    return m_rkTriangle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLinComp2Triangle2<Real>::Test ()
{
    Real afDist[3];
    int aiSign[3], iPositive, iNegative, iZero;
    TriangleLineRelations(afDist,aiSign,iPositive,iNegative,iZero);

    if ( iPositive == 3 || iNegative == 3 )
    {
        m_iIntersectionType = IT_EMPTY;
    }
    else
    {
        Real afParam[2];
        GetInterval(afDist,aiSign,afParam);

        Intersector1<Real> kIntr(afParam[0],afParam[1],
            m_rkComponent.GetMin(),m_rkComponent.GetMax());

        kIntr.Find();

        int iQuantity = kIntr.GetQuantity();
        if ( iQuantity == 2 )
            m_iIntersectionType = IT_SEGMENT;
        else if ( iQuantity == 1 )
            m_iIntersectionType = IT_POINT;
        else
            m_iIntersectionType = IT_EMPTY;
    }

    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrLinComp2Triangle2<Real>::Find ()
{
    Real afDist[3];
    int aiSign[3], iPositive, iNegative, iZero;
    TriangleLineRelations(afDist,aiSign,iPositive,iNegative,iZero);

    if ( iPositive == 3 || iNegative == 3 )
    {
        // empty set
        m_kIntrComponent.SetInterval((Real)1.0,(Real)-1.0);
    }
    else
    {
        Real afParam[2];
        GetInterval(afDist,aiSign,afParam);

        Intersector1<Real> kIntr(afParam[0],afParam[1],
            m_rkComponent.GetMin(),m_rkComponent.GetMax());

        kIntr.Find();

        int iQuantity = kIntr.GetQuantity();
        if ( iQuantity == 2 )
        {
            // segment
            m_kIntrComponent.Origin = m_rkComponent.Origin;
            m_kIntrComponent.Direction = m_rkComponent.Direction;
            m_kIntrComponent.SetInterval(kIntr.GetOverlap(0),
                kIntr.GetOverlap(1));
        }
        else if ( iQuantity == 1 )
        {
            // point
            m_kIntrComponent.Origin = m_rkComponent.Origin;
            m_kIntrComponent.Direction = m_rkComponent.Direction;
            m_kIntrComponent.SetInterval(kIntr.GetOverlap(0),
                kIntr.GetOverlap(0));
        }
        else
        {
            // empty set
            m_kIntrComponent.SetInterval((Real)1.0,(Real)-1.0);
        }
    }

    m_kIntrComponent.MakeCanonical();
    m_iIntersectionType = m_kIntrComponent.GetType();
    return m_iIntersectionType != IT_EMPTY;
}
//----------------------------------------------------------------------------
template <class Real>
const LinComp2<Real>&
IntrLinComp2Triangle2<Real>::GetIntersectionComponent () const
{
    return m_kIntrComponent;
}
//----------------------------------------------------------------------------
template <class Real>
void IntrLinComp2Triangle2<Real>::TriangleLineRelations (Real afDist[3],
    int aiSign[3], int& riPositive, int& riNegative, int& riZero)
{
    riPositive = 0;
    riNegative = 0;
    riZero = 0;
    for (int i = 0; i < 3; i++)
    {
        Vector2<Real> kDiff = m_rkTriangle.V[i] - m_rkComponent.Origin;
        afDist[i] = kDiff.DotPerp(m_rkComponent.Direction);
        if ( afDist[i] > Math<Real>::ZERO_TOLERANCE )
        {
            aiSign[i] = 1;
            riPositive++;
        }
        else if ( afDist[i] < -Math<Real>::ZERO_TOLERANCE )
        {
            aiSign[i] = -1;
            riNegative++;
        }
        else
        {
            afDist[i] = (Real)0.0;
            aiSign[i] = 0;
            riZero++;
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void IntrLinComp2Triangle2<Real>::GetInterval (const Real afDist[3],
    const int aiSign[3], Real afParam[2])
{
    // project triangle onto line
    Real afProj[3];
    int i;
    for (i = 0; i < 3; i++)
    {
        Vector2<Real> kDiff = m_rkTriangle.V[i] - m_rkComponent.Origin;
        afProj[i] = m_rkComponent.Direction.Dot(kDiff);
    }

    // compute transverse intersections of triangle edges with line
    Real fNumer, fDenom;
    int i0, i1, i2;
    int iQuantity = 0;
    for (i0 = 2, i1 = 0; i1 < 3; i0 = i1++)
    {
        if ( aiSign[i0]*aiSign[i1] < 0 )
        {
            assert( iQuantity < 2 );
            fNumer = afDist[i0]*afProj[i1] - afDist[i1]*afProj[i0];
            fDenom = afDist[i0] - afDist[i1];
            afParam[iQuantity++] = fNumer/fDenom;
        }
    }

    // check for grazing contact
    if ( iQuantity < 2 )
    {
        for (i0 = 1, i1 = 2, i2 = 0; i2 < 3; i0 = i1, i1 = i2, i2++)
        {
            if ( aiSign[i2] == 0 )
            {
                assert( iQuantity < 2 );
                fNumer = afDist[i0]*afProj[i1] - afDist[i1]*afProj[i0];
                fDenom = afDist[i0] - afDist[i1];
                afParam[iQuantity++] = fNumer/fDenom;
            }
        }
    }

    // sort
    assert( iQuantity >= 1 );
    if ( iQuantity == 2 )
    {
        if ( afParam[0] > afParam[1] )
        {
            Real fSave = afParam[0];
            afParam[0] = afParam[1];
            afParam[1] = fSave;
        }
    }
    else
    {
        afParam[1] = afParam[0];
    }
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template class IntrLinComp2Triangle2<float>;

// double
template class IntrLinComp2Triangle2<double>;
//----------------------------------------------------------------------------
}
