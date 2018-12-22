// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "TriangulateInt.h"

//----------------------------------------------------------------------------
TriangulateInt::TriangulateInt (int iQuantity, int** aaiPosition,
    int* aiTriangles)
{
    // assert:
    //   iQuantity >= 3
    //   aaiPosition has iQuantity elements
    //   aiTriangles has 3*(iQuantity-2) elements
    m_iQuantity = iQuantity;
    m_aaiPosition = aaiPosition;
    int i, iQm1 = iQuantity - 1;

    m_akVertex = new Vertex[iQuantity];
    m_iCFirst = -1;
    m_iCLast = -1;
    m_iRFirst = -1;
    m_iRLast = -1;
    m_iEFirst = -1;
    m_iELast = -1;

    // Create a circular list of the polygon vertices for dynamic removal of
    // vertices.  Keep track of two linear sublists, one for the convex
    // vertices and one for the reflex vertices.  This is an O(N) process
    // where N is the number of polygon vertices.
    for (i = 0; i <= iQm1; i++)
    {
        // link the vertices
        Vertex& rkV = V(i);
        rkV.m_iVPrev = ( i > 0 ? i-1 : iQm1 );
        rkV.m_iVNext = ( i < iQm1 ? i+1 : 0 );

        // link the convex/reflex vertices
        IsConvex(i) ? InsertAfterC(i) : InsertAfterR(i);
    }

    if ( m_iRFirst == -1 )
    {
        // polygon is convex, just triangle fan it
        for (i = 1; i < iQm1; i++)
        {
            *aiTriangles++ = 0;
            *aiTriangles++ = i;
            *aiTriangles++ = i+1;
        }
        return;
    }

    // Identify the ears and build a circular list of them.  Let V0, V1, and
    // V2 be consecutive vertices forming a triangle T.  The vertex V1 is an
    // ear if no other vertices of the polygon lie inside T.  Although it is
    // enough to show that V1 is not an ear by finding at least one other
    // vertex inside T, it is sufficient to search only the reflex vertices.
    // This is an O(C*R) process where C is the number of convex vertices and
    // R is the number of reflex vertices, N = C+R.  The order is O(N^2), for
    // example when C = R = N/2.
    for (i = m_iCFirst; i != -1; i = V(i).m_iSNext)
    {
        if  ( IsEar(i) )
            InsertEndE(i);
    }
    V(m_iEFirst).m_iEPrev = m_iELast;
    V(m_iELast).m_iENext = m_iEFirst;

    // Remove the ears, one at a time.
    while ( true )
    {
        // add triangle of ear to output
        int iVPrev = V(m_iEFirst).m_iVPrev;
        int iVNext = V(m_iEFirst).m_iVNext;
        *aiTriangles++ = iVPrev;
        *aiTriangles++ = m_iEFirst;
        *aiTriangles++ = iVNext;

        // remove the vertex corresponding to the ear
        RemoveV(m_iEFirst);
        if ( --iQuantity == 3 )
        {
            // Only one triangle remains, just remove the ear and copy it.
            RemoveE();
            iVPrev = V(m_iEFirst).m_iVPrev;
            iVNext = V(m_iEFirst).m_iVNext;
            *aiTriangles++ = iVPrev;
            *aiTriangles++ = m_iEFirst;
            *aiTriangles++ = iVNext;
            break;
        }

        // removal of the ear can cause an adjacent vertex to become an ear
        bool bWasReflex;

        Vertex& rkVPrev = V(iVPrev);
        if ( !rkVPrev.m_bIsEar )
        {
            bWasReflex = !rkVPrev.m_bIsConvex;
            if ( IsConvex(iVPrev) )
            {
                if ( bWasReflex )
                    RemoveR(iVPrev);

                if ( IsEar(iVPrev) )
                    InsertBeforeE(iVPrev);
            }
        }

        Vertex& rkVNext = V(iVNext);
        if ( !rkVNext.m_bIsEar )
        {
            bWasReflex = !rkVNext.m_bIsConvex;
            if ( IsConvex(iVNext) )
            {
                if ( bWasReflex )
                    RemoveR(iVNext);

                if ( IsEar(iVNext) )
                    InsertAfterE(iVNext);
            }
        }

        // remove the ear
        RemoveE();
    }
}
//----------------------------------------------------------------------------
TriangulateInt::~TriangulateInt ()
{
    delete[] m_akVertex;
}
//----------------------------------------------------------------------------
bool TriangulateInt::IsConvex (int i)
{
    Vertex& rkV = V(i);
    int iP = rkV.m_iVPrev;
    int iN = rkV.m_iVNext;
    rkV.m_bIsConvex = IsLeftOf(m_aaiPosition[iN],m_aaiPosition[iP],
        m_aaiPosition[i]);
    return rkV.m_bIsConvex;
}
//----------------------------------------------------------------------------
bool TriangulateInt::IsEar (int i)
{
    Vertex& rkV = V(i);

    if ( m_iRFirst == -1 )
    {
        // The remaining polygon is convex.
        rkV.m_bIsEar = true;
        return true;
    }

    // Search the reflex vertices and test if any are in the triangle
    // <V[prev],V[i],V[next]>.
    rkV.m_bIsEar = true;
    for (int j = m_iRFirst; j != -1; j = V(j).m_iSNext)
    {
        int iP = rkV.m_iVPrev;
        int iN = rkV.m_iVNext;
        bool bContained = IsInTriangle(m_aaiPosition[j],m_aaiPosition[iP],
            m_aaiPosition[i],m_aaiPosition[iN]);

        if ( bContained )
        {
            rkV.m_bIsEar = false;
            break;
        }
    }

    return rkV.m_bIsEar;
}
//----------------------------------------------------------------------------
void TriangulateInt::InsertAfterC (int i)
{
    if ( m_iCFirst == -1 )
    {
        // add first convex vertex
        m_iCFirst = i;
    }
    else
    {
        V(m_iCLast).m_iSNext = i;
        V(i).m_iSPrev = m_iCLast;
    }
    m_iCLast = i;
}
//----------------------------------------------------------------------------
void TriangulateInt::InsertAfterR (int i)
{
    if ( m_iRFirst == -1 )
    {
        // add first reflex vertex
        m_iRFirst = i;
    }
    else
    {
        V(m_iRLast).m_iSNext = i;
        V(i).m_iSPrev = m_iRLast;
    }
    m_iRLast = i;
}
//----------------------------------------------------------------------------
void TriangulateInt::InsertEndE (int i)
{
    if ( m_iEFirst == -1 )
    {
        // add first ear
        m_iEFirst = i;
        m_iELast = i;
    }
    V(m_iELast).m_iENext = i;
    V(i).m_iEPrev = m_iELast;
    m_iELast = i;
}
//----------------------------------------------------------------------------
void TriangulateInt::InsertAfterE (int i)
{
    Vertex& rkVFirst = V(m_iEFirst);
    int iCurrENext = rkVFirst.m_iENext;
    Vertex& rkV = V(i);
    rkV.m_iEPrev = m_iEFirst;
    rkV.m_iENext = iCurrENext;
    rkVFirst.m_iENext = i;
    V(iCurrENext).m_iEPrev = i;
}
//----------------------------------------------------------------------------
void TriangulateInt::InsertBeforeE (int i)
{
    Vertex& rkVFirst = V(m_iEFirst);
    int iCurrEPrev = rkVFirst.m_iEPrev;
    Vertex& rkV = V(i);
    rkV.m_iEPrev = iCurrEPrev;
    rkV.m_iENext = m_iEFirst;
    rkVFirst.m_iEPrev = i;
    V(iCurrEPrev).m_iENext = i;
}
//----------------------------------------------------------------------------
void TriangulateInt::RemoveV (int i)
{
    int iCurrVPrev = V(i).m_iVPrev;
    int iCurrVNext = V(i).m_iVNext;
    V(iCurrVPrev).m_iVNext = iCurrVNext;
    V(iCurrVNext).m_iVPrev = iCurrVPrev;
}
//----------------------------------------------------------------------------
void TriangulateInt::RemoveE ()
{
    int iCurrEPrev = V(m_iEFirst).m_iEPrev;
    int iCurrENext = V(m_iEFirst).m_iENext;
    V(iCurrEPrev).m_iENext = iCurrENext;
    V(iCurrENext).m_iEPrev = iCurrEPrev;
    m_iEFirst = iCurrENext;
}
//----------------------------------------------------------------------------
void TriangulateInt::RemoveR (int i)
{
    assert( m_iRFirst != -1 && m_iRLast != -1 );

    if ( i == m_iRFirst )
    {
        m_iRFirst = V(i).m_iSNext;
        if ( m_iRFirst != -1 )
            V(m_iRFirst).m_iSPrev = -1;
        V(i).m_iSNext = -1;
    }
    else if ( i == m_iRLast )
    {
        m_iRLast = V(i).m_iSPrev;
        if ( m_iRLast != -1 )
            V(m_iRLast).m_iSNext = -1;
        V(i).m_iSPrev = -1;
    }
    else
    {
        int iCurrSPrev = V(i).m_iSPrev;
        int iCurrSNext = V(i).m_iSNext;
        V(iCurrSPrev).m_iSNext = iCurrSNext;
        V(iCurrSNext).m_iSPrev = iCurrSPrev;
        V(i).m_iSNext = -1;
        V(i).m_iSPrev = -1;
    }
}
//----------------------------------------------------------------------------
TriangulateInt::Vertex& TriangulateInt::V (int i)
{
    assert( 0 <= i && i < m_iQuantity );
    return m_akVertex[i];
}
//----------------------------------------------------------------------------
bool TriangulateInt::IsLeftOf (const int aiP[2], const int aiV0[2],
    const int aiV1[2]) const
{
    int iDotPerp =
        (aiV1[0] - aiV0[0])*(aiP[1] - aiV0[1]) -
        (aiV1[1] - aiV0[1])*(aiP[0] - aiV0[0]);

    return iDotPerp > 0;
}
//----------------------------------------------------------------------------
bool TriangulateInt::IsInTriangle (const int aiP[2], const int aiV0[2],
    const int aiV1[2], const int aiV2[2]) const
{
    return IsLeftOf(aiP,aiV0,aiV1)
        && IsLeftOf(aiP,aiV1,aiV2)
        && IsLeftOf(aiP,aiV2,aiV0);
}
//----------------------------------------------------------------------------
