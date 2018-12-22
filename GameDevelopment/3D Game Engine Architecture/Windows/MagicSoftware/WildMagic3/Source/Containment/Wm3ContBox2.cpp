// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ContBox2.h"
#include "Wm3ApprGaussPointsFit2.h"
#include "Wm3ContConvexHull2.h"

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Box2<Real> ContAlignedBox (int iQuantity, const Vector2<Real>* akPoint,
    const bool* abValid)
{
    Box2<Real> kBox(Vector2<Real>::ZERO,Vector2<Real>::UNIT_X,
        Vector2<Real>::UNIT_Y,(Real)1.0,(Real)1.0);

    int i;

    Vector2<Real> kMin, kMax;
    if ( !abValid )
    {
        kMin = akPoint[0];
        kMax = kMin;

        for (i = 1; i < iQuantity; i++)
        {
            if ( akPoint[i].X() < kMin.X() )
                kMin.X() = akPoint[i].X();
            else if ( akPoint[i].X() > kMax.X() )
                kMax.X() = akPoint[i].X();

            if ( akPoint[i].Y() < kMin.Y() )
                kMin.Y() = akPoint[i].Y();
            else if ( akPoint[i].Y() > kMax.Y() )
                kMax.Y() = akPoint[i].Y();
        }
    }
    else
    {
        for (i = 0; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                kMin = akPoint[i];
                kMax = kMin;
                break;
            }
        }
        if ( i == iQuantity )
        {
            kBox.Extent[0] = (Real)-1.0;
            kBox.Extent[1] = (Real)-1.0;
            return kBox;
        }

        for (++i; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                if ( akPoint[i].X() < kMin.X() )
                    kMin.X() = akPoint[i].X();
                else if ( akPoint[i].X() > kMax.X() )
                    kMax.X() = akPoint[i].X();

                if ( akPoint[i].Y() < kMin.Y() )
                    kMin.Y() = akPoint[i].Y();
                else if ( akPoint[i].Y() > kMax.Y() )
                    kMax.Y() = akPoint[i].Y();
            }
        }
    }

    kBox.Center = ((Real)0.5)*(kMin + kMax);
    Vector2<Real> kHalfDiagonal = ((Real)0.5)*(kMax - kMin);
    for (i = 0; i < 2; i++)
        kBox.Extent[i] = kHalfDiagonal[i];

    return kBox;
}
//----------------------------------------------------------------------------
template <class Real>
Box2<Real> ContOrientedBox (int iQuantity, const Vector2<Real>* akPoint,
    const bool* abValid)
{
    Box2<Real> kBox = GaussPointsFit2<Real>(iQuantity,akPoint,abValid);
    if ( kBox.Extent[0] == (Real)-1.0 )
        return kBox;

    // Let C be the box center and let U0 and U1 be the box axes.  Each
    // input point is of the form X = C + y0*U0 + y1*U1.  The following code
    // computes min(y0), max(y0), min(y1), max(y1), min(y2), and max(y2).
    // The box center is then adjusted to be
    //   C' = C + 0.5*(min(y0)+max(y0))*U0 + 0.5*(min(y1)+max(y1))*U1

    Vector2<Real> kDiff;
    Real fY0Min = (Real)0.0, fY0Max = (Real)0.0;
    Real fY1Min = (Real)0.0, fY1Max = (Real)0.0;
    Real fY0, fY1;
    int i;

    if ( !abValid )
    {
        kDiff = akPoint[0] - kBox.Center;
        fY0Max = fY0Min = kDiff.Dot(kBox.Axis[0]);
        fY1Max = fY1Min = kDiff.Dot(kBox.Axis[1]);

        for (i = 1; i < iQuantity; i++)
        {
            kDiff = akPoint[i] - kBox.Center;

            fY0 = kDiff.Dot(kBox.Axis[0]);
            if ( fY0 < fY0Min )
                fY0Min = fY0;
            else if ( fY0 > fY0Max )
                fY0Max = fY0;

            fY1 = kDiff.Dot(kBox.Axis[1]);
            if ( fY1 < fY1Min )
                fY1Min = fY1;
            else if ( fY1 > fY1Max )
                fY1Max = fY1;
        }
    }
    else
    {
        for (i = 0; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                kDiff = akPoint[i] - kBox.Center;
                fY0Max = fY0Min = kDiff.Dot(kBox.Axis[0]);
                fY1Max = fY1Min = kDiff.Dot(kBox.Axis[1]);
                break;
            }
        }

        for (++i; i < iQuantity; i++)
        {
            if ( abValid[i] )
            {
                kDiff = akPoint[i] - kBox.Center;

                fY0 = kDiff.Dot(kBox.Axis[0]);
                if ( fY0 < fY0Min )
                    fY0Min = fY0;
                else if ( fY0 > fY0Max )
                    fY0Max = fY0;

                fY1 = kDiff.Dot(kBox.Axis[1]);
                if ( fY1 < fY1Min )
                    fY1Min = fY1;
                else if ( fY1 > fY1Max )
                    fY1Max = fY1;
            }
        }
    }

    kBox.Center +=
        (((Real)0.5)*(fY0Min+fY0Max))*kBox.Axis[0] +
        (((Real)0.5)*(fY1Min+fY1Max))*kBox.Axis[1];

    kBox.Extent[0] = ((Real)0.5)*(fY0Max - fY0Min);
    kBox.Extent[1] = ((Real)0.5)*(fY1Max - fY1Min);

    return kBox;
}
//----------------------------------------------------------------------------
template <class Real>
Box2<Real> ContMinBox (int iQuantity, const Vector2<Real>* akPoint,
    bool bIsConvexPolygon)
{
    Box2<Real> kBox;

    // get the convex hull of the points
    Vector2<Real>* akHPoint = 0;
    if ( bIsConvexPolygon )
    {
        akHPoint = (Vector2<Real>*)akPoint;
    }
    else
    {
        ConvexHull2<Real> kHull(iQuantity,akPoint,true);
        const int* aiIndex = kHull.GetIndices();

        if ( kHull.GetType() == ConvexHull2<Real>::HULL_POINT )
        {
            kBox.Center = akPoint[aiIndex[0]];
            kBox.Axis[0] = Vector2<Real>::UNIT_X;
            kBox.Axis[1] = Vector2<Real>::UNIT_Y;
            kBox.Extent[0] = (Real)0.0;
            kBox.Extent[1] = (Real)0.0;
            return kBox;
        }

        if ( kHull.GetType() == ConvexHull2<Real>::HULL_LINEAR )
        {
            kBox.Center = ((Real)0.5)*(akPoint[aiIndex[0]] +
                akPoint[aiIndex[1]]);
            Vector2<Real> kDiff = akPoint[aiIndex[1]] - akPoint[aiIndex[0]];
            kBox.Extent[0] = ((Real)0.5)*kDiff.Normalize();
            kBox.Extent[1] = (Real)0.0;
            kBox.Axis[0] = kDiff;
            kBox.Axis[1] = -kBox.Axis[0].Perp();
            return kBox;
        }

        iQuantity = kHull.GetQuantity();
        akHPoint = new Vector2<Real>[iQuantity];
        for (int i = 0; i < iQuantity; i++)
            akHPoint[i] = akPoint[aiIndex[i]];
    }

    // The input points are V[0] through V[N-1] and are assumed to be the
    // vertices of a convex polygon that are counterclockwise ordered.  The
    // input points must not contain three consecutive collinear points.

    // Unit-length edge directions of convex polygon.  These could be
    // precomputed and passed to this routine if the application requires it.
    int iQuantityM1 = iQuantity -1;
    Vector2<Real>* akEdge = new Vector2<Real>[iQuantity];
    bool* abVisited = new bool[iQuantity];
    int i;
    for (i = 0; i < iQuantityM1; i++)
    {
        akEdge[i] = akHPoint[i+1] - akHPoint[i];
        akEdge[i].Normalize();
        abVisited[i] = false;
    }
    akEdge[iQuantityM1] = akHPoint[0] - akHPoint[iQuantityM1];
    akEdge[iQuantityM1].Normalize();
    abVisited[iQuantityM1] = false;

    // Find the smallest axis-aligned box containing the points.  Keep track
    // of the extremum indices, L (left), R (right), B (bottom), and T (top)
    // so that the following constraints are met:
    //   V[L].X() <= V[i].X() for all i and V[(L+1)%N].X() > V[L].X()
    //   V[R].X() >= V[i].X() for all i and V[(R+1)%N].X() < V[R].X()
    //   V[B].Y() <= V[i].Y() for all i and V[(B+1)%N].Y() > V[B].Y()
    //   V[T].Y() >= V[i].Y() for all i and V[(T+1)%N].Y() < V[R].Y()
    Real fXMin = akHPoint[0].X(), fXMax = fXMin;
    Real fYMin = akHPoint[0].Y(), fYMax = fYMin;
    int iLIndex = 0, iRIndex = 0, iBIndex = 0, iTIndex = 0;
    for (i = 1; i < iQuantity; i++)
    {
        if ( akHPoint[i].X() <= fXMin )
        {
            fXMin = akHPoint[i].X();
            iLIndex = i;
        }
        else if ( akHPoint[i].X() >= fXMax )
        {
            fXMax = akHPoint[i].X();
            iRIndex = i;
        }

        if ( akHPoint[i].Y() <= fYMin )
        {
            fYMin = akHPoint[i].Y();
            iBIndex = i;
        }
        else if ( akHPoint[i].Y() >= fYMax )
        {
            fYMax = akHPoint[i].Y();
            iTIndex = i;
        }
    }

    // wrap-around tests to ensure the constraints mentioned above
    if ( akHPoint[0].X() <= fXMin )
    {
        fXMin = akHPoint[0].X();
        iLIndex = 0;
    }
    else if ( akHPoint[0].X() >= fXMax )
    {
        fXMax = akHPoint[0].X();
        iRIndex = 0;
    }

    if ( akHPoint[0].Y() <= fYMin )
    {
        fYMin = akHPoint[0].Y();
        iBIndex = 0;
    }
    else if ( akHPoint[0].Y() >= fYMax )
    {
        fYMax = akHPoint[0].Y();
        iTIndex = 0;
    }

    // dimensions of axis-aligned box (extents store width and height for now)
    kBox.Center.X() = ((Real)0.5)*(fXMin + fXMax);
    kBox.Center.Y() = ((Real)0.5)*(fYMin + fYMax);
    kBox.Axis[0] = Vector2<Real>::UNIT_X;
    kBox.Axis[1] = Vector2<Real>::UNIT_Y;
    kBox.Extent[0] = ((Real)0.5)*(fXMax - fXMin);
    kBox.Extent[1] = ((Real)0.5)*(fYMax - fYMin);
    Real fMinAreaDiv4 = kBox.Extent[0]*kBox.Extent[1];

    // rotating calipers algorithm
    enum { F_NONE, F_LEFT, F_RIGHT, F_BOTTOM, F_TOP };
    Vector2<Real> kU = Vector2<Real>::UNIT_X, kV = Vector2<Real>::UNIT_Y;

    bool bDone = false;
    while ( !bDone )
    {
        // determine edge that forms smallest angle with current box edges
        int iFlag = F_NONE;
        Real fMaxDot = (Real)0.0;

        Real fDot = kU.Dot(akEdge[iBIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_BOTTOM;
        }

        fDot = kV.Dot(akEdge[iRIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_RIGHT;
        }

        fDot = -kU.Dot(akEdge[iTIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_TOP;
        }

        fDot = -kV.Dot(akEdge[iLIndex]);
        if ( fDot > fMaxDot )
        {
            fMaxDot = fDot;
            iFlag = F_LEFT;
        }

        switch ( iFlag )
        {
        case F_BOTTOM:
            if ( abVisited[iBIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute box axes with E[B] as an edge
                kU = akEdge[iBIndex];
                kV = -kU.Perp();

                // mark edge visited and rotate the calipers
                abVisited[iBIndex] = true;
                if ( ++iBIndex == iQuantity )
                    iBIndex = 0;
            }
            break;
        case F_RIGHT:
            if ( abVisited[iRIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute dimensions of box with E[R] as an edge
                kV = akEdge[iRIndex];
                kU = kV.Perp();

                // mark edge visited and rotate the calipers
                abVisited[iRIndex] = true;
                if ( ++iRIndex == iQuantity )
                    iRIndex = 0;
            }
            break;
        case F_TOP:
            if ( abVisited[iTIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute dimensions of box with E[T] as an edge
                kU = -akEdge[iTIndex];
                kV = -kU.Perp();

                // mark edge visited and rotate the calipers
                abVisited[iTIndex] = true;
                if ( ++iTIndex == iQuantity )
                    iTIndex = 0;
            }
            break;
        case F_LEFT:
            if ( abVisited[iLIndex] )
            {
                bDone = true;
            }
            else
            {
                // compute dimensions of box with E[L] as an edge
                kV = -akEdge[iLIndex];
                kU = kV.Perp();

                // mark edge visited and rotate the calipers
                abVisited[iLIndex] = true;
                if ( ++iLIndex == iQuantity )
                    iLIndex = 0;
            }
            break;
        case F_NONE:
            // polygon is a rectangle
            bDone = true;
            break;
        }

        Real fExtent0 = ((Real)0.5)*(kU.Dot(akHPoint[iRIndex] -
            akHPoint[iLIndex]));
        Real fExtent1 = ((Real)0.5)*(kV.Dot(akHPoint[iTIndex] -
            akHPoint[iBIndex]));
        Real fAreaDiv4 = fExtent0*fExtent1;
        if ( fAreaDiv4 < fMinAreaDiv4 )
        {
            fMinAreaDiv4 = fAreaDiv4;
            kBox.Axis[0] = kU;
            kBox.Axis[1] = kV;
            kBox.Extent[0] = fExtent0;
            kBox.Extent[1] = fExtent1;

            // compute box center
            Vector2<Real> kTmp = ((Real)0.5)*(akHPoint[iTIndex] +
                akHPoint[iBIndex]) - akHPoint[iLIndex];
            kBox.Center = akHPoint[iLIndex] + fExtent0*kBox.Axis[0] +
                (kBox.Axis[1].Dot(kTmp))*kBox.Axis[1];
        }
    }

    delete[] abVisited;
    delete[] akEdge;
    if ( !bIsConvexPolygon )
        delete[] akHPoint;

    return kBox;
}
//----------------------------------------------------------------------------
template <class Real>
bool InBox (const Vector2<Real>& rkPoint, const Box2<Real>& rkBox)
{
    Vector2<Real> kDiff = rkPoint - rkBox.Center;
    for (int i = 0; i < 2; i++)
    {
        Real fCoeff = kDiff.Dot(rkBox.Axis[i]);
        if ( Math<Real>::FAbs(fCoeff) > rkBox.Extent[i] )
            return false;
    }
    return true;
}
//----------------------------------------------------------------------------
template <class Real>
Box2<Real> MergeBoxes (const Box2<Real>& rkBox0, const Box2<Real>& rkBox1)
{
    // construct a box that contains the input boxes
    Box2<Real> kBox;

    // The first guess at the box center.  This value will be updated later
    // after the input box vertices are projected onto axes determined by an
    // average of box axes.
    kBox.Center = ((Real)0.5)*(rkBox0.Center + rkBox1.Center);

    // The merged box axes are the averages of the input box axes.  The
    // axes of the second box are negated, if necessary, so they form acute
    // angles with the axes of the first box.
    if ( rkBox0.Axis[0].Dot(rkBox1.Axis[0]) >= (Real)0.0 )
    {
        kBox.Axis[0] = ((Real)0.5)*(rkBox0.Axis[0] + rkBox1.Axis[0]);
        kBox.Axis[0].Normalize();
    }
    else
    {
        kBox.Axis[0] = ((Real)0.5)*(rkBox0.Axis[0] - rkBox1.Axis[0]);
        kBox.Axis[0].Normalize();
    }
    kBox.Axis[1] = -kBox.Axis[0].Perp();

    // Project the input box vertices onto the merged-box axes.  Each axis
    // D[i] containing the current center C has a minimum projected value
    // pmin[i] and a maximum projected value pmax[i].  The corresponding end
    // points on the axes are C+pmin[i]*D[i] and C+pmax[i]*D[i].  The point C
    // is not necessarily the midpoint for any of the intervals.  The actual
    // box center will be adjusted from C to a point C' that is the midpoint
    // of each interval,
    //   C' = C + sum_{i=0}^1 0.5*(pmin[i]+pmax[i])*D[i]
    // The box extents are
    //   e[i] = 0.5*(pmax[i]-pmin[i])

    int i, j;
    Real fDot;
    Vector2<Real> akVertex[4], kDiff;
    Vector2<Real> kMin = Vector2<Real>::ZERO;
    Vector2<Real> kMax = Vector2<Real>::ZERO;

    rkBox0.ComputeVertices(akVertex);
    for (i = 0; i < 4; i++)
    {
        kDiff = akVertex[i] - kBox.Center;
        for (j = 0; j < 2; j++)
        {
            fDot = kDiff.Dot(kBox.Axis[j]);
            if ( fDot > kMax[j] )
                kMax[j] = fDot;
            else if ( fDot < kMin[j] )
                kMin[j] = fDot;
        }
    }

    rkBox1.ComputeVertices(akVertex);
    for (i = 0; i < 4; i++)
    {
        kDiff = akVertex[i] - kBox.Center;
        for (j = 0; j < 1; j++)
        {
            fDot = kDiff.Dot(kBox.Axis[j]);
            if ( fDot > kMax[j] )
                kMax[j] = fDot;
            else if ( fDot < kMin[j] )
                kMin[j] = fDot;
        }
    }

    // [kMin,kMax] is the axis-aligned box in the coordinate system of the
    // merged box axes.  Update the current box center to be the center of
    // the new box.  Compute the extens based on the new center.
    for (j = 0; j < 2; j++)
    {
        kBox.Center += kBox.Axis[j]*(((Real)0.5)*(kMax[j]+kMin[j]));
        kBox.Extent[j] = ((Real)0.5)*(kMax[j]-kMin[j]);
    }

    return kBox;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
Box2<float> ContAlignedBox<float> (int, const Vector2<float>*,
    const bool*);

template WM3_ITEM
Box2<float> ContOrientedBox<float> (int, const Vector2<float>*,
    const bool*);

template WM3_ITEM
Box2<float> ContMinBox<float> (int, const Vector2<float>*, bool);

template WM3_ITEM
bool InBox<float> (const Vector2<float>&, const Box2<float>&);

template WM3_ITEM
Box2<float> MergeBoxes<float> (const Box2<float>&, const Box2<float>&);

// double
template WM3_ITEM
Box2<double> ContAlignedBox<double> (int, const Vector2<double>*,
    const bool*);

template WM3_ITEM
Box2<double> ContOrientedBox<double> (int, const Vector2<double>*,
    const bool*);

template WM3_ITEM
Box2<double> ContMinBox<double> (int, const Vector2<double>*, bool);

template WM3_ITEM
bool InBox<double> (const Vector2<double>&, const Box2<double>&);

template WM3_ITEM
Box2<double> MergeBoxes<double> (const Box2<double>&, const Box2<double>&);
//----------------------------------------------------------------------------
}
