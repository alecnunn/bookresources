// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3DistSegment3Segment3.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
DistSegment3Segment3<Real>::DistSegment3Segment3 (
    const Segment3<Real>& rkSegment0, const Segment3<Real>& rkSegment1)
    :
    m_rkSegment0(rkSegment0),
    m_rkSegment1(rkSegment1)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistSegment3Segment3<Real>::GetSegment0 () const
{
    return m_rkSegment0;
}
//----------------------------------------------------------------------------
template <class Real>
const Segment3<Real>& DistSegment3Segment3<Real>::GetSegment1 () const
{
    return m_rkSegment1;
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Segment3<Real>::Get ()
{
    Real fSqrDist = GetSquared();
    return Math<Real>::Sqrt(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Segment3<Real>::GetSquared ()
{
    Vector3<Real> kDiff = m_rkSegment0.Origin - m_rkSegment1.Origin;
    Real fA01 = -m_rkSegment0.Direction.Dot(m_rkSegment1.Direction);
    Real fB0 = kDiff.Dot(m_rkSegment0.Direction);
    Real fC = kDiff.SquaredLength();
    Real fDet = Math<Real>::FAbs((Real)1.0 - fA01*fA01);
    Real fB1, fS0, fS1, fSqrDist, fTmp;

    if ( fDet >= Math<Real>::ZERO_TOLERANCE )
    {
        // line segments are not parallel
        fB1 = -kDiff.Dot(m_rkSegment1.Direction);
        fS0 = fA01*fB1-fB0;
        fS1 = fA01*fB0-fB1;
        
        if ( fS0 >= (Real)0.0 )
        {
            if ( fS0 <= fDet )
            {
                if ( fS1 >= (Real)0.0 )
                {
                    if ( fS1 <= fDet )  // region 0 (interior)
                    {
                        // minimum at two interior points of 3D lines
                        Real fInvDet = ((Real)1.0)/fDet;
                        fS0 *= fInvDet;
                        fS1 *= fInvDet;
                        fSqrDist = fS0*(fS0+fA01*fS1+((Real)2.0)*fB0) +
                            fS1*(fA01*fS0+fS1+((Real)2.0)*fB1)+fC;
                    }
                    else  // region 3 (side)
                    {
                        fS1 = (Real)1.0;
                        fTmp = fA01+fB0;
                        if ( fTmp >= (Real)0.0 )
                        {
                            fS0 = (Real)0.0;
                            fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                        }
                        else if ( -fTmp >= (Real)1.0 )
                        {
                            fS0 = (Real)1.0;
                            fSqrDist = (Real)2.0+fC+((Real)2.0)*(fB1+fTmp);
                        }
                        else
                        {
                            fS0 = -fTmp;
                            fSqrDist = fTmp*fS0+(Real)1.0+((Real)2.0)*fB1+fC;
                        }
                    }
                }
                else  // region 7 (side)
                {
                    fS1 = (Real)0.0;
                    if ( fB0 >= (Real)0.0 )
                    {
                        fS0 = (Real)0.0;
                        fSqrDist = fC;
                    }
                    else if ( -fB0 >= (Real)1.0 )
                    {
                        fS0 = (Real)1.0;
                        fSqrDist = (Real)1.0+((Real)2.0)*fB0+fC;
                    }
                    else
                    {
                        fS0 = -fB0;
                        fSqrDist = fB0*fS0+fC;
                    }
                }
            }
            else
            {
                if ( fS1 >= (Real)0.0 )
                {
                    if ( fS1 <= fDet )  // region 1 (side)
                    {
                        fS0 = (Real)1.0;
                        fTmp = fA01+fB1;
                        if ( fTmp >= (Real)0.0 )
                        {
                            fS1 = (Real)0.0;
                            fSqrDist = (Real)1.0+((Real)2.0)*fB0+fC;
                        }
                        else if ( -fTmp >= (Real)1.0 )
                        {
                            fS1 = (Real)1.0;
                            fSqrDist = (Real)2.0+fC+((Real)2.0)*(fB0+fTmp);
                        }
                        else
                        {
                            fS1 = -fTmp;
                            fSqrDist = fTmp*fS1+(Real)1.0+((Real)2.0)*fB0+fC;
                        }
                    }
                    else  // region 2 (corner)
                    {
                        fTmp = fA01+fB0;
                        if ( -fTmp <= (Real)1.0 )
                        {
                            fS1 = (Real)1.0;
                            if ( fTmp >= (Real)0.0 )
                            {
                                fS0 = (Real)0.0;
                                fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                            }
                            else
                            {
                                 fS0 = -fTmp;
                                 fSqrDist = fTmp*fS0+(Real)1.0+
                                     ((Real)2.0)*fB1+fC;
                            }
                        }
                        else
                        {
                            fS0 = (Real)1.0;
                            fTmp = fA01+fB1;
                            if ( fTmp >= (Real)0.0 )
                            {
                                fS1 = (Real)0.0;
                                fSqrDist = (Real)1.0+((Real)2.0)*fB0+fC;
                            }
                            else if ( -fTmp >= (Real)1.0 )
                            {
                                fS1 = (Real)1.0;
                                fSqrDist = (Real)2.0+fC+
                                    ((Real)2.0)*(fB0+fTmp);
                            }
                            else
                            {
                                fS1 = -fTmp;
                                fSqrDist = fTmp*fS1+(Real)1.0+
                                    ((Real)2.0)*fB0+fC;
                            }
                        }
                    }
                }
                else  // region 8 (corner)
                {
                    if ( -fB0 < (Real)1.0 )
                    {
                        fS1 = (Real)0.0;
                        if ( fB0 >= (Real)0.0 )
                        {
                            fS0 = (Real)0.0;
                            fSqrDist = fC;
                        }
                        else
                        {
                            fS0 = -fB0;
                            fSqrDist = fB0*fS0+fC;
                        }
                    }
                    else
                    {
                        fS0 = (Real)1.0;
                        fTmp = fA01+fB1;
                        if ( fTmp >= (Real)0.0 )
                        {
                            fS1 = (Real)0.0;
                            fSqrDist = (Real)1.0+((Real)2.0)*fB0+fC;
                        }
                        else if ( -fTmp >= (Real)1.0 )
                        {
                            fS1 = (Real)1.0;
                            fSqrDist = (Real)2.0+fC+((Real)2.0)*(fB0+fTmp);
                        }
                        else
                        {
                            fS1 = -fTmp;
                            fSqrDist = fTmp*fS1+(Real)1.0+((Real)2.0)*fB0+fC;
                        }
                    }
                }
            }
        }
        else 
        {
            if ( fS1 >= (Real)0.0 )
            {
                if ( fS1 <= fDet )  // region 5 (side)
                {
                    fS0 = (Real)0.0;
                    if ( fB1 >= (Real)0.0 )
                    {
                        fS1 = (Real)0.0;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= (Real)1.0 )
                    {
                        fS1 = (Real)1.0;
                        fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                    }
                    else
                    {
                        fS1 = -fB1;
                        fSqrDist = fB1*fS1+fC;
                    }
                }
                else  // region 4 (corner)
                {
                    fTmp = fA01+fB0;
                    if ( fTmp < (Real)0.0 )
                    {
                        fS1 = (Real)1.0;
                        if ( -fTmp >= (Real)1.0 )
                        {
                            fS0 = (Real)1.0;
                            fSqrDist = (Real)2.0+fC+((Real)2.0)*(fB1+fTmp);
                        }
                        else
                        {
                            fS0 = -fTmp;
                            fSqrDist = fTmp*fS0+(Real)1.0+((Real)2.0)*fB1+fC;
                        }
                    }
                    else
                    {
                        fS0 = (Real)0.0;
                        if ( fB1 >= (Real)0.0 )
                        {
                            fS1 = (Real)0.0;
                            fSqrDist = fC;
                        }
                        else if ( -fB1 >= (Real)1.0 )
                        {
                            fS1 = (Real)1.0;
                            fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                        }
                        else
                        {
                            fS1 = -fB1;
                            fSqrDist = fB1*fS1+fC;
                        }
                    }
                }
            }
            else   // region 6 (corner)
            {
                if ( fB0 < (Real)0.0 )
                {
                    fS1 = (Real)0.0;
                    if ( -fB0 >= (Real)1.0 )
                    {
                        fS0 = (Real)1.0;
                        fSqrDist = (Real)1.0+((Real)2.0)*fB0+fC;
                    }
                    else
                    {
                        fS0 = -fB0;
                        fSqrDist = fB0*fS0+fC;
                    }
                }
                else
                {
                    fS0 = (Real)0.0;
                    if ( fB1 >= (Real)0.0 )
                    {
                        fS1 = (Real)0.0;
                        fSqrDist = fC;
                    }
                    else if ( -fB1 >= (Real)1.0 )
                    {
                        fS1 = (Real)1.0;
                        fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                    }
                    else
                    {
                        fS1 = -fB1;
                        fSqrDist = fB1*fS1+fC;
                    }
                }
            }
        }
    }
    else
    {
        // line segments are parallel
        if ( fA01 > (Real)0.0 )
        {
            // direction vectors form an obtuse angle
            if ( fB0 >= (Real)0.0 )
            {
                fS0 = (Real)0.0;
                fS1 = (Real)0.0;
                fSqrDist = fC;
            }
            else if ( -fB0 <= (Real)1.0 )
            {
                fS0 = -fB0;
                fS1 = (Real)0.0;
                fSqrDist = fB0*fS0+fC;
            }
            else
            {
                fB1 = -kDiff.Dot(m_rkSegment1.Direction);
                fS0 = (Real)1.0;
                fTmp = (Real)1.0+fB0;
                if ( -fTmp >= fA01 )
                {
                    fS1 = (Real)1.0;
                    fSqrDist = (Real)2.0+fC+((Real)2.0)*(fA01+fB0+fB1);
                }
                else
                {
                    fS1 = -fTmp/fA01;
                    fSqrDist = (Real)1.0+((Real)2.0)*fB0+fC+fS1*(fS1+
                        ((Real)2.0)*(fA01+fB1));
                }
            }
        }
        else
        {
            // direction vectors form an acute angle
            if ( -fB0 >= (Real)1.0 )
            {
                fS0 = (Real)1.0;
                fS1 = (Real)0.0;
                fSqrDist = (Real)1.0+((Real)2.0)*fB0+fC;
            }
            else if ( fB0 <= (Real)0.0 )
            {
                fS0 = -fB0;
                fS1 = (Real)0.0;
                fSqrDist = fB0*fS0+fC;
            }
            else
            {
                fB1 = -kDiff.Dot(m_rkSegment1.Direction);
                fS0 = (Real)0.0;
                if ( fB0 >= -fA01 )
                {
                    fS1 = (Real)1.0;
                    fSqrDist = (Real)1.0+((Real)2.0)*fB1+fC;
                }
                else
                {
                    fS1 = -fB0/fA01;
                    fSqrDist = fC+fS1*(((Real)2.0)*fB1+fS1);
                }
            }
        }
    }

    m_kClosestPoint0 = m_rkSegment0.Origin + fS0*m_rkSegment0.Direction;
    m_kClosestPoint1 = m_rkSegment1.Origin + fS1*m_rkSegment1.Direction;
    return Math<Real>::FAbs(fSqrDist);
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Segment3<Real>::Get (Real fS1,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkSegment0.Origin + fS1*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkSegment1.Origin + fS1*rkVelocity1;
    Segment3<Real> kMSegment0(kMOrigin0,m_rkSegment0.Direction,
        m_rkSegment0.Extent);
    Segment3<Real> kMSegment1(kMOrigin1,m_rkSegment1.Direction,
        m_rkSegment1.Extent);
    return DistSegment3Segment3<Real>(kMSegment0,kMSegment1).Get();
}
//----------------------------------------------------------------------------
template <class Real>
Real DistSegment3Segment3<Real>::GetSquared (Real fS1,
    const Vector3<Real>& rkVelocity0, const Vector3<Real>& rkVelocity1)
{
    Vector3<Real> kMOrigin0 = m_rkSegment0.Origin + fS1*rkVelocity0;
    Vector3<Real> kMOrigin1 = m_rkSegment1.Origin + fS1*rkVelocity1;
    Segment3<Real> kMSegment0(kMOrigin0,m_rkSegment0.Direction,
        m_rkSegment0.Extent);
    Segment3<Real> kMSegment1(kMOrigin1,m_rkSegment1.Direction,
        m_rkSegment1.Extent);
    return DistSegment3Segment3<Real>(kMSegment0,kMSegment1).GetSquared();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class DistSegment3Segment3<float>;

// double
template WM3_ITEM
class DistSegment3Segment3<double>;
//----------------------------------------------------------------------------
}
