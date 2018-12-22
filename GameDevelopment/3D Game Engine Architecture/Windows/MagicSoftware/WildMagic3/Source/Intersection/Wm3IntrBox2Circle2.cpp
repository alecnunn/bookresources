// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3IntrBox2Circle2.h"
#include "Wm3DistVector2Box2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
IntrBox2Circle2<Real>::IntrBox2Circle2 (const Box2<Real>& rkBox,
    const Circle2<Real>& rkCircle)
    :
    m_rkBox(rkBox),
    m_rkCircle(rkCircle)
{
}
//----------------------------------------------------------------------------
template <class Real>
const Box2<Real>& IntrBox2Circle2<Real>::GetBox () const
{
    return m_rkBox;
}
//----------------------------------------------------------------------------
template <class Real>
const Circle2<Real>& IntrBox2Circle2<Real>::GetCircle () const
{
    return m_rkCircle;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox2Circle2<Real>::Test ()
{
    Real fDistance = DistVector2Box2<Real>(m_rkCircle.Center,m_rkBox).Get();
    return fDistance <= m_rkCircle.Radius;
}
//----------------------------------------------------------------------------
template <class Real>
bool IntrBox2Circle2<Real>::Find (Real fTMax,
    const Vector2<Real>& rkVelocity0, const Vector2<Real>& rkVelocity1)
{
    // convert circle center to box coordinates
    Vector2<Real> kDiff = m_rkCircle.Center - m_rkBox.Center;
    Vector2<Real> kVel = rkVelocity1 - rkVelocity0;
    Real fCx = kDiff.Dot(m_rkBox.Axis[0]);
    Real fCy = kDiff.Dot(m_rkBox.Axis[1]);
    Real fVx = kVel.Dot(m_rkBox.Axis[0]);
    Real fVy = kVel.Dot(m_rkBox.Axis[1]);
    Real fEx = m_rkBox.Extent[0];
    Real fEy = m_rkBox.Extent[1];
    Real fIx, fIy;

    int iType = 0;

    if ( fCx < -fEx )
    {
        if ( fCy < -fEy )
        {
            // region Rmm
            iType = TestVertexRegion(fCx,fCy,fVx,fVy,fEx,fEy,fIx,fIy);
        }
        else if ( fCy <= fEy )
        {
            // region Rmz
            iType = TestEdgeRegion(fCx,fCy,fVx,fVy,fEx,fEy,fIx,fIy);
        }
        else
        {
            // region Rmp
            iType = TestVertexRegion(fCx,-fCy,fVx,-fVy,fEx,fEy,fIx,fIy);
            fIy = -fIy;
        }
    }
    else if ( fCx <= fEx )
    {
        if ( fCy < -fEy )
        {
            // region Rzm
            iType = TestEdgeRegion(fCy,fCx,fVy,fVx,fEy,fEx,fIy,fIx);
        }
        else if ( fCy <= fEy )
        {
            // region Rzz: The circle is already intersecting the box.  Use
            // the circle center as the intersection point, but let the
            // caller know of that the objects overlap by returning an "other"
            // intersection.
            m_fContactTime = (Real)0.0;
            m_kContactPoint = m_rkCircle.Center;
            m_iIntersectionType = IT_OTHER;
            return true;
        }
        else
        {
            // region Rzp
            iType = TestEdgeRegion(-fCy,fCx,-fVy,fVx,fEy,fEx,fIy,fIx);
            fIy = -fIy;
        }
    }
    else
    {
        if ( fCy < -fEy )
        {
            // region Rpm
            iType = TestVertexRegion(-fCx,fCy,-fVx,fVy,fEx,fEy,fIx,fIy);
            fIx = -fIx;
        }
        else if ( fCy <= fEy )
        {
            // region Rpz
            iType = TestEdgeRegion(-fCx,fCy,-fVx,fVy,fEx,fEy,fIx,fIy);
            fIx = -fIx;
        }
        else
        {
            // region Rpp
            iType = TestVertexRegion(-fCx,-fCy,-fVx,-fVy,fEx,fEy,fIx,fIy);
            fIx = -fIx;
            fIy = -fIy;
        }
    }

    if ( iType != 1 )
    {
        m_iIntersectionType = IT_EMPTY;
        return false;
    }

    m_kContactPoint = m_rkBox.Center+fIx*m_rkBox.Axis[0]+fIy*m_rkBox.Axis[1];
    m_iIntersectionType = IT_POINT;
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>& IntrBox2Circle2<Real>::GetContactPoint () const
{
    return m_kContactPoint;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox2Circle2<Real>::TestVertexRegion (Real fCx, Real fCy, Real fVx,
    Real fVy, Real fEx, Real fEy, Real& rfIx, Real& rfIy)
{
    Real fDx = fCx + fEx;
    Real fDy = fCy + fEy;
    Real fRSqr = m_rkCircle.Radius*m_rkCircle.Radius;
    Real fDiff = fDx*fDx + fDy*fDy - fRSqr;
    if ( fDiff <= (Real)0.0 )
    {
        // circle is already intersecting the box
        m_fContactTime = (Real)0.0;
        return -1;
    }

    Real fDot = fVx*fDx + fVy*fDy;
    if ( fDot >= (Real)0.0 )
    {
        // circle not moving towards box
        return 0;
    }

    Real fDotPerp = fVx*fDy - fVy*fDx;
    Real fVSqr, fInv;

    if ( fDotPerp >= (Real)0.0 )
    {
        // potential contact on left edge
        if ( fDotPerp <= m_rkCircle.Radius*fVy )
        {
            // lower left corner is first point of contact
            rfIx = -fEx;
            rfIy = -fEy;
            fVSqr = fVx*fVx + fVy*fVy;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            return 1;
        }

        if ( fVx <= (Real)0.0 )
        {
            // passed corner, moving away from box
            return 0;
        }

        fVSqr = fVx*fVx + fVy*fVy;
        fDy = fCy - fEy;
        fDotPerp = fVx*fDy - fVy*fDx;
        if ( fDotPerp >= 0.0f && fDotPerp*fDotPerp > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if ( fDotPerp <= m_rkCircle.Radius*fVy )
        {
            // first contact on left edge of box
            m_fContactTime = -(fDx+m_rkCircle.Radius)/fVx;
            rfIy = fCy + m_fContactTime*fVy;
        }
        else
        {
            // first contact at upper left corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIy = fEy;
        }
    }
    else
    {
        // potential contact on bottom edge
        if ( -fDotPerp <= m_rkCircle.Radius*fVx )
        {
            // lower left corner is first point of contact
            rfIx = -fEx;
            rfIy = -fEy;
            fVSqr = fVx*fVx + fVy*fVy;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            return 1;
        }

        if ( fVy <= (Real)0.0 )
        {
            // passed corner, moving away from box
            return 0;
        }

        fVSqr = fVx*fVx + fVy*fVy;
        fDx = fCx - fEx;
        fDotPerp = fVx*fDy - fVy*fDx;
        if ( -fDotPerp >= (Real)0.0 && fDotPerp*fDotPerp > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with y = ymin.
        rfIy = -fEy;

        if ( -fDotPerp <= m_rkCircle.Radius*fVx )
        {
            // first contact on bottom edge of box
            m_fContactTime = -(fDy+m_rkCircle.Radius)/fVy;
            rfIx = fCx + m_fContactTime*fVx;
        }
        else
        {
            // first contact at lower right corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIx = fEx;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------
template <class Real>
int IntrBox2Circle2<Real>::TestEdgeRegion (Real fCx, Real fCy, Real fVx,
    Real fVy, Real fEx, Real fEy, Real& rfIx, Real& rfIy)
{
    Real fDx = fCx + fEx;
    Real fXSignedDist = fDx + m_rkCircle.Radius;
    if ( fXSignedDist >= (Real)0.0 )
    {
        // circle is already intersecting the box
        m_fContactTime = (Real)0.0;
        return -1;
    }

    if ( fVx <= (Real)0.0 )
    {
        // circle not moving towards box
        return 0;
    }

    Real fRSqr = m_rkCircle.Radius*m_rkCircle.Radius;
    Real fVSqr = fVx*fVx + fVy*fVy;
    Real fDy, fDot, fDotPerp, fDiff, fInv;

    if ( fVy >= (Real)0.0 )
    {
        fDy = fCy - fEy;
        fDotPerp = fVx*fDy - fVy*fDx;
        if ( fDotPerp >= (Real)0.0 && fDotPerp*fDotPerp > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if ( fDotPerp <= m_rkCircle.Radius*fVy )
        {
            // first contact on left edge of box
            m_fContactTime = -fXSignedDist/fVx;
            rfIy = fCy + m_fContactTime*fVy;
        }
        else
        {
            // first contact at corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIy = fEy;
        }
    }
    else
    {
        fDy = fCy + fEy;
        fDotPerp = fVx*fDy - fVy*fDx;
        if ( fDotPerp <= (Real)0.0 && fDotPerp*fDotPerp > fRSqr*fVSqr )
        {
            // circle misses box
            return 0;
        }

        // Circle will intersect box.  Determine first time and place of
        // contact with x = xmin.
        rfIx = -fEx;

        if ( fDotPerp >= m_rkCircle.Radius*fVy )
        {
            // first contact on left edge of box
            m_fContactTime = -fXSignedDist/fVx;
            rfIy = fCy + m_fContactTime*fVy;
        }
        else
        {
            // first contact at corner of box
            fDot = fVx*fDx + fVy*fDy;
            fDiff = fDx*fDx + fDy*fDy - fRSqr;
            fInv = Math<Real>::InvSqrt(Math<Real>::FAbs(
                fDot*fDot-fVSqr*fDiff));
            m_fContactTime = fDiff*fInv/((Real)1.0-fDot*fInv);
            rfIy = -fEy;
        }
    }

    return 1;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class IntrBox2Circle2<float>;

// double
template WM3_ITEM
class IntrBox2Circle2<double>;
//----------------------------------------------------------------------------
}
