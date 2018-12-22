// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3Delaunay2.h"
#include <algorithm>

namespace Wm3
{
//----------------------------------------------------------------------------
template <class Real>
Delaunay2<Real>::Delaunay2 (int iVQuantity, Vector2<Real>* akVertex,
    bool bOwner)
    :
    m_kVertex(iVQuantity)
{
    m_iVQuantity = iVQuantity;
    m_akVertex = akVertex;
    m_bOwner = bOwner;

    // output values
    m_iTQuantity = 0;
    m_aiIndex = 0;
    m_aiAdjacent = 0;

    // transform the points to [-1,1]^2 for numerical preconditioning
    Real fMin = akVertex[0].X(), fMax = fMin;
    int i;
    for (i = 0; i < iVQuantity; i++)
    {
        if ( akVertex[i].X() < fMin )
            fMin = akVertex[i].X();
        else if ( akVertex[i].X() > fMax )
            fMax = akVertex[i].X();

        if ( akVertex[i].Y() < fMin )
            fMin = akVertex[i].Y();
        else if ( akVertex[i].Y() > fMax )
            fMax = akVertex[i].Y();
    }
    Real fHalfRange = ((Real)0.5)*(fMax - fMin);
    Real fInvHalfRange = ((Real)1.0)/fHalfRange;

    // sort by x-component to prepare to remove duplicate vertices
    for (i = 0; i < iVQuantity; i++)
    {
        m_kVertex[i].V.X() = -(Real)1.0 + fInvHalfRange*(akVertex[i].X() -
            fMin);
        m_kVertex[i].V.Y() = -(Real)1.0 + fInvHalfRange*(akVertex[i].Y() -
            fMin);
        m_kVertex[i].Index = i;
    }
    std::sort(m_kVertex.begin(),m_kVertex.end());

    // remove duplicate points
    typename VArray::iterator pkEnd =
        std::unique(m_kVertex.begin(),m_kVertex.end());
    m_kVertex.erase(pkEnd,m_kVertex.end());

    // construct supertriangle and add to triangle mesh
    iVQuantity = (int)m_kVertex.size();
    m_aiSuperV[0] = iVQuantity;
    m_aiSuperV[1] = iVQuantity+1;
    m_aiSuperV[2] = iVQuantity+2;
    m_kVertex.push_back(Vertex(Vector2<Real>(-(Real)2.0,-(Real)2.0),
        m_aiSuperV[0]));
    m_kVertex.push_back(Vertex(Vector2<Real>(+(Real)5.0,-(Real)2.0),
        m_aiSuperV[1]));
    m_kVertex.push_back(Vertex(Vector2<Real>(-(Real)2.0,+(Real)5.0),
        m_aiSuperV[2]));

    Triangle* pkTri = new Triangle(m_aiSuperV[0],m_aiSuperV[1],
        m_aiSuperV[2],0,0,0);
    m_kTriangle.insert(pkTri);
    m_apkSuperT[0] = pkTri;
    m_apkSuperT[1] = pkTri;
    m_apkSuperT[2] = pkTri;

    // incrementally update the triangulation
    for (i = 0; i < iVQuantity; i++)
    {
        // construct the insertion polygon
        TSet kPolyTri;
        GetInsertionPolygon(m_kVertex[i].V,kPolyTri);

        EArray kPoly;
        GetInsertionPolygonEdges(kPolyTri,kPoly);

        // add new triangles formed by the point and insertion polygon edges
        AddTriangles(i,kPoly);

        // remove insertion polygon
        RemoveInsertionPolygon(kPolyTri);
    }

    // remove triangles sharing a vertex of the supertriangle
    RemoveTriangles();

    // assign integer values to the triangles for use by the caller
    std::map<Triangle*,int> kPermute;
    TSetIterator pkTIter = m_kTriangle.begin();
    for (i = 0; pkTIter != m_kTriangle.end(); pkTIter++)
    {
        pkTri = *pkTIter;
        kPermute[pkTri] = i++;
    }
    kPermute[0] = -1;

    // put Delaunay triangles into an array (vertices and adjacency info)
    m_iTQuantity = (int)m_kTriangle.size();
    if ( m_iTQuantity > 0 )
    {
        m_aiIndex = new int[3*m_iTQuantity];
        m_aiAdjacent = new int[3*m_iTQuantity];
        i = 0;
        pkTIter = m_kTriangle.begin();
        for (/**/; pkTIter != m_kTriangle.end(); pkTIter++)
        {
            pkTri = *pkTIter;
            m_aiIndex[i] = m_kVertex[pkTri->V[0]].Index;
            m_aiAdjacent[i++] = kPermute[pkTri->A[0]];
            m_aiIndex[i] = m_kVertex[pkTri->V[1]].Index;
            m_aiAdjacent[i++] = kPermute[pkTri->A[1]];
            m_aiIndex[i] = m_kVertex[pkTri->V[2]].Index;
            m_aiAdjacent[i++] = kPermute[pkTri->A[2]];
        }
        assert( i == 3*m_iTQuantity );
    }
}
//----------------------------------------------------------------------------
template <class Real>
Delaunay2<Real>::~Delaunay2 ()
{
    TSetIterator pkTIter = m_kTriangle.begin();
    for (/**/; pkTIter != m_kTriangle.end(); pkTIter++)
        delete *pkTIter;

    delete[] m_aiIndex;
    delete[] m_aiAdjacent;

    if ( m_bOwner )
        delete[] m_akVertex;
}
//----------------------------------------------------------------------------
template <class Real>
int Delaunay2<Real>::GetVertexQuantity () const
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const Vector2<Real>* Delaunay2<Real>::GetVertices () const
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
template <class Real>
int Delaunay2<Real>::GetTriangleQuantity () const
{
    return m_iTQuantity;
}
//----------------------------------------------------------------------------
template <class Real>
const int* Delaunay2<Real>::GetIndices () const
{
    return m_aiIndex;
}
//----------------------------------------------------------------------------
template <class Real>
const int* Delaunay2<Real>::GetAdjacencies () const
{
    return m_aiAdjacent;
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay2<Real>::GetHull (int& riEQuantity, int*& raiIndex)
{
    riEQuantity = 0;
    raiIndex = 0;

    int i, iAdjQuantity = 3*m_iTQuantity;
    for (i = 0; i < iAdjQuantity; i++)
    {
        if ( m_aiAdjacent[i] == -1 )
            riEQuantity++;
    }
    assert( riEQuantity > 0 );
    if ( riEQuantity == 0 )
        return;

    raiIndex = new int[2*riEQuantity];
    int* piIndex = raiIndex;

    for (i = 0; i < iAdjQuantity; i++)
    {
        if ( m_aiAdjacent[i] == -1 )
        {
            *piIndex++ = m_aiIndex[i];
            *piIndex++ = m_aiIndex[(i+1)%3];
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
int Delaunay2<Real>::GetContainingTriangle (const Vector2<Real>& rkP) const
{
    // start at first triangle in mesh
    int iIndex = 0;

    // use triangle edges as binary separating lines
    for (int i = 0; i < m_iTQuantity; i++)
    {
        Vector2<Real> kV0 = m_akVertex[m_aiIndex[3*iIndex]];
        Vector2<Real> kV1 = m_akVertex[m_aiIndex[3*iIndex+1]];
        Vector2<Real> kV2 = m_akVertex[m_aiIndex[3*iIndex+2]];

        if ( PointOutsideEdge(rkP,kV0,kV1) )
        {
            iIndex = m_aiAdjacent[3*iIndex];
            if ( iIndex == -1 )
                return -1;
            continue;
        }

        if ( PointOutsideEdge(rkP,kV1,kV2) )
        {
            iIndex = m_aiAdjacent[3*iIndex+1];
            if ( iIndex == -1 )
                return -1;
            continue;
        }

        if ( PointOutsideEdge(rkP,kV2,kV0) )
        {
            iIndex = m_aiAdjacent[3*iIndex+2];
            if ( iIndex == -1 )
                return -1;
            continue;
        }

        return iIndex;
    }

    return -1;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::GetVertices (int i, Vector2<Real>& rkV0,
    Vector2<Real>& rkV1, Vector2<Real>& rkV2) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        rkV0 = m_akVertex[m_aiIndex[3*i  ]];
        rkV1 = m_akVertex[m_aiIndex[3*i+1]];
        rkV2 = m_akVertex[m_aiIndex[3*i+2]];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::GetIndices (int i, int aiIndex[3]) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        aiIndex[0] = m_aiIndex[3*i  ];
        aiIndex[1] = m_aiIndex[3*i+1];
        aiIndex[2] = m_aiIndex[3*i+2];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::GetAdjacencies (int i, int aiAdjacent[3]) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        aiAdjacent[0] = m_aiAdjacent[3*i  ];
        aiAdjacent[1] = m_aiAdjacent[3*i+1];
        aiAdjacent[2] = m_aiAdjacent[3*i+2];
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::GetBarycentrics (int i, const Vector2<Real>& rkP,
    Real afBary[3]) const
{
    if ( 0 <= i && i < m_iTQuantity )
    {
        Vector2<Real> kV0 = m_akVertex[m_aiIndex[3*i  ]];
        Vector2<Real> kV1 = m_akVertex[m_aiIndex[3*i+1]];
        Vector2<Real> kV2 = m_akVertex[m_aiIndex[3*i+2]];
        rkP.GetBarycentrics(kV0,kV1,kV2,afBary);
        return true;
    }

    return false;
}
//----------------------------------------------------------------------------
template <class Real>
typename Delaunay2<Real>::Triangle*
Delaunay2<Real>::GetContaining (const Vector2<Real>& rkP) const
{
    // This is a linear search through the triangle fan with origin at
    // supervertex S1.  The opposite edges of the triangles are those edges of
    // the current triangulation that are visible to S1.  You could attempt to
    // do a binary search instead, but the bottleneck in this implementation
    // is the construction of the insertion polygon.

    // start with supertriangle <S0,S1,V>
    Triangle* pkTri = m_apkSuperT[0];
    assert( pkTri->V[0] == m_aiSuperV[0] && pkTri->V[1] == m_aiSuperV[1] );

    const Vector2<Real>& rkS1 = m_kVertex[m_aiSuperV[1]].V;
    Vector2<Real> kDiff = rkP - rkS1;
    int iSIndex = 1;
    int iVIndex = 2;  // local index following that of S1

    // The search should never iterate over more than all the triangles.  But
    // just to be safe...
    for (int i = 0; i < (int)m_kTriangle.size(); i++)
    {
        // test if P is inside the triangle
        Vector2<Real> kVmS1 = m_kVertex[pkTri->V[iVIndex]].V - rkS1;
        Real fDotPerp = kVmS1.X()*kDiff.Y() - kVmS1.Y()*kDiff.X();
        if ( fDotPerp >= (Real)0.0 )
            return pkTri;

        pkTri = pkTri->A[iSIndex];
        assert( pkTri );
        if ( pkTri->V[0] == m_aiSuperV[1] )
        {
            iSIndex = 0;
            iVIndex = 1;
        }
        else if ( pkTri->V[1] == m_aiSuperV[1] )
        {
            iSIndex = 1;
            iVIndex = 2;
        }
        else if ( pkTri->V[2] == m_aiSuperV[1] )
        {
            iSIndex = 2;
            iVIndex = 0;
        }
        else
        {
            assert( false );
        }
    }

    // by construction, point must be in some triangle in the mesh
    assert( false );
    return pkTri;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::IsInsertionComponent (const Vector2<Real>& rkV,
    Triangle* pkTri) const
{
    // determine the number of vertices in common with the supertriangle
    int iCommon = 0, j = -1;
    for (int i = 0; i < 3; i++)
    {
        int iV = pkTri->V[i];
        if ( iV == m_aiSuperV[0] )
        {
            iCommon++;
            j = i;
        }
        if ( iV == m_aiSuperV[1] )
        {
            iCommon++;
            j = i;
        }
        if ( iV == m_aiSuperV[2] )
        {
            iCommon++;
            j = i;
        }
    }

    if ( iCommon == 0 )
    {
        return PointInCircle(rkV,m_kVertex[pkTri->V[0]].V,
            m_kVertex[pkTri->V[1]].V,m_kVertex[pkTri->V[2]].V);
    }

    if ( iCommon == 1 )
    {
        return !PointOutsideEdge(rkV,m_kVertex[pkTri->V[(j+1)%3]].V,
            m_kVertex[pkTri->V[(j+2)%3]].V);
    }

    return PointInTriangle(rkV,m_kVertex[pkTri->V[0]].V,
        m_kVertex[pkTri->V[1]].V,m_kVertex[pkTri->V[2]].V);
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay2<Real>::GetInsertionPolygon (const Vector2<Real>& rkV,
    TSet& rkPolyTri) const
{
    // locate triangle containing input point, add to insertion polygon
    Triangle* pkTri = GetContaining(rkV);
    //assert( IsInsertionComponent(rkV,pkTri) );
    rkPolyTri.insert(pkTri);

    // breadth-first search for other triangles in the insertion polygon
    TSet kInterior, kBoundary;
    kInterior.insert(pkTri);
    Triangle* pkAdj;
    int i;
    for (i = 0; i < 3; i++)
    {
        pkAdj = pkTri->A[i];
        if ( pkAdj )
            kBoundary.insert(pkAdj);
    }

    while ( kBoundary.size() > 0 )
    {
        TSet kExterior;

        // process boundary triangles
        TSetIterator pkIter;
        for (pkIter = kBoundary.begin(); pkIter != kBoundary.end(); pkIter++)
        {
            // current boundary triangle to process
            pkTri = *pkIter;
            if ( IsInsertionComponent(rkV,pkTri) )
            {
                // triangle is an insertion component
                rkPolyTri.insert(pkTri);

                // locate adjacent, exterior triangles for later processing
                for (i = 0; i < 3; i++)
                {
                    pkAdj = pkTri->A[i];
                    if ( pkAdj
                    &&   kInterior.find(pkAdj) == kInterior.end()
                    &&   kBoundary.find(pkAdj) == kBoundary.end() )
                    {
                        kExterior.insert(pkAdj);
                    }
                }
            }
        }

        // boundary triangles processed, move them to interior
        for (pkIter = kBoundary.begin(); pkIter != kBoundary.end(); pkIter++)
            kInterior.insert(*pkIter);

        // exterior triangles are next in line to be processed
        kBoundary = kExterior;
    }

    // If the input point is outside the current convex hull, triangles
    // sharing a supertriangle edge have to be added to the insertion polygon
    // if the non-supertriangle vertex is shared by two edges that are visible
    // to the input point.
    for (i = 0; i < 3; i++)
    {
        pkTri = m_apkSuperT[i];
        if ( rkPolyTri.find(pkTri->A[1]) != rkPolyTri.end()
        &&   rkPolyTri.find(pkTri->A[2]) != rkPolyTri.end() )
        {
            rkPolyTri.insert(pkTri);
        }
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay2<Real>::GetInsertionPolygonEdges (TSet& rkPolyTri,
    EArray& rkPoly) const
{
    // locate edges of the insertion polygon
    EMap kIPolygon;
    int iV0, iV1;
    Triangle* pkTri;
    Triangle* pkAdj;
    TSetIterator pkTIter;
    for (pkTIter = rkPolyTri.begin(); pkTIter != rkPolyTri.end(); pkTIter++)
    {
        // get an insertion polygon triangle
        pkTri = *pkTIter;

        // If an adjacent triangle is not an insertion polygon triangle, then
        // the shared edge is a boundary edge of the insertion polygon.  Use
        // this edge to create a new Delaunay triangle from the input vertex
        // and the shared edge.
        for (int j = 0; j < 3; j++)
        {
            pkAdj = pkTri->A[j];
            if ( pkAdj )
            {
                if ( rkPolyTri.find(pkAdj) == rkPolyTri.end() )
                {
                    // adjacent triangle is not part of insertion polygon
                    iV0 = pkTri->V[j];
                    iV1 = pkTri->V[(j+1)%3];
                    kIPolygon[iV0] = Edge(iV0,iV1,pkTri,pkAdj);
                }
            }
            else
            {
                // no adjacent triangle, edge is part of insertion polygon
                iV0 = pkTri->V[j];
                iV1 = pkTri->V[(j+1)%3];
                kIPolygon[iV0] = Edge(iV0,iV1,pkTri,pkAdj);
            }
        }
    }

    // insertion polygon should be at least a triangle
    int iSize = (int)kIPolygon.size();
    assert( iSize >= 3 );

    // sort the edges
    typename EMap::iterator pkE = kIPolygon.begin();
    iV0 = pkE->second.V0;
    for (int i = 0; i < iSize; i++)
    {
        iV1 = pkE->second.V1;
        rkPoly.push_back(pkE->second);
        pkE = kIPolygon.find(iV1);
        assert( pkE != kIPolygon.end() );
    }
    assert( iV1 == iV0 );
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay2<Real>::AddTriangles (int iV2, const EArray& rkPoly)
{
    // create new triangles
    int iSize = (int)rkPoly.size();
    TArray kTriangle(iSize);
    Triangle* pkTri;
    Triangle* pkAdj;
    int i, j;
    for (i = 0; i < iSize; i++)
    {
        const Edge& rkE = rkPoly[i];

        // construct new triangle
        pkTri = new Triangle(rkE.V0,rkE.V1,iV2,rkE.A,0,0);
        kTriangle[i] = pkTri;
        if ( rkE.V0 == m_aiSuperV[0] && rkE.V1 == m_aiSuperV[1] )
            m_apkSuperT[0] = pkTri;
        else if ( rkE.V0 == m_aiSuperV[1] && rkE.V1 == m_aiSuperV[2] )
            m_apkSuperT[1] = pkTri;
        else if ( rkE.V0 == m_aiSuperV[2] && rkE.V1 == m_aiSuperV[0] )
            m_apkSuperT[2] = pkTri;

        // update information of triangle adjacent to insertion polygon
        pkAdj = rkE.A;
        if ( pkAdj )
        {
            for (j = 0; j < 3; j++)
            {
                if ( pkAdj->A[j] == rkE.T )
                {
                    pkAdj->A[j] = pkTri;
                    break;
                }
            }
        }
    }

    // Insertion polygon is a star polygon with center vertex V2.  Finalize
    // the triangles by setting the adjacency information.
    for (i = 0; i < iSize; i++)
    {
        // current triangle
        pkTri = kTriangle[i];

        // connect to next new triangle
        int iNext = i+1;
        if ( iNext == iSize )
            iNext = 0;
        pkTri->A[1] = kTriangle[iNext];

        // connect to previous new triangle
        int iPrev = i-1;
        if ( iPrev == -1 )
            iPrev = iSize-1;
        pkTri->A[2] = kTriangle[iPrev];
    }

    // add the triangles to the mesh
    for (i = 0; i < iSize; i++)
        m_kTriangle.insert(kTriangle[i]);
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay2<Real>::RemoveInsertionPolygon (TSet& rkPolyTri)
{
    TSetIterator pkTIter;
    for (pkTIter = rkPolyTri.begin(); pkTIter != rkPolyTri.end(); pkTIter++)
    {
        Triangle* pkTri = *pkTIter;
        m_kTriangle.erase(pkTri);
        delete pkTri;
    }
}
//----------------------------------------------------------------------------
template <class Real>
void Delaunay2<Real>::RemoveTriangles ()
{
    // identify those triangles sharing a vertex of the supertriangle
    TSet kRemoveTri;
    Triangle* pkTri;
    TSetIterator pkTIter = m_kTriangle.begin();
    for (/**/; pkTIter != m_kTriangle.end(); pkTIter++)
    {
        pkTri = *pkTIter;
        for (int j = 0; j < 3; j++)
        {
            int iV = pkTri->V[j];
            if ( iV == m_aiSuperV[0]
            ||   iV == m_aiSuperV[1]
            ||   iV == m_aiSuperV[2] )
            {
                kRemoveTri.insert(pkTri);
                break;
            }
        }
    }

    // remove the triangles from the mesh
    pkTIter = kRemoveTri.begin();
    for (/**/; pkTIter != kRemoveTri.end(); pkTIter++)
    {
        pkTri = *pkTIter;
        for (int j = 0; j < 3; j++)
        {
            // break the links with adjacent triangles
            Triangle* pkAdj = pkTri->A[j];
            if ( pkAdj )
            {
                for (int k = 0; k < 3; k++)
                {
                    if ( pkAdj->A[k] == pkTri )
                    {
                        pkAdj->A[k] = 0;
                        break;
                    }
                }
            }
        }
        m_kTriangle.erase(pkTri);
        delete pkTri;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::PointInCircle (const Vector2<Real>& rkP,
    const Vector2<Real>& rkV0, const Vector2<Real>& rkV1,
    const Vector2<Real>& rkV2)
{
    // assert: <V0,V1,V2> is counterclockwise ordered
    double dV0x = (double)rkV0.X();
    double dV0y = (double)rkV0.Y();
    double dV1x = (double)rkV1.X();
    double dV1y = (double)rkV1.Y();
    double dV2x = (double)rkV2.X();
    double dV2y = (double)rkV2.Y();
    double dV3x = (double)rkP.X();
    double dV3y = (double)rkP.Y();

    double dR0Sqr = dV0x*dV0x + dV0y*dV0y;
    double dR1Sqr = dV1x*dV1x + dV1y*dV1y;
    double dR2Sqr = dV2x*dV2x + dV2y*dV2y;
    double dR3Sqr = dV3x*dV3x + dV3y*dV3y;

    double dDiff1x = dV1x - dV0x;
    double dDiff1y = dV1y - dV0y;
    double dRDiff1 = dR1Sqr - dR0Sqr;
    double dDiff2x = dV2x - dV0x;
    double dDiff2y = dV2y - dV0y;
    double dRDiff2 = dR2Sqr - dR0Sqr;
    double dDiff3x = dV3x - dV0x;
    double dDiff3y = dV3y - dV0y;
    double dRDiff3 = dR3Sqr - dR0Sqr;

    double dDet =
        dDiff1x*(dDiff2y*dRDiff3 - dRDiff2*dDiff3y) -
        dDiff1y*(dDiff2x*dRDiff3 - dRDiff2*dDiff3x) +
        dRDiff1*(dDiff2x*dDiff3y - dDiff2y*dDiff3x);

    return dDet <= 0.0;
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::PointOutsideEdge (const Vector2<Real>& rkP,
    const Vector2<Real>& rkV0, const Vector2<Real>& rkV1)
{
    // Test if P is on the right of the line V0+t*(V1-V0).  The theoretical
    // test is as follows.  Let D = V1-V0 = (Dx,Dy) and N = (-Dy,Dx).  The
    // vector N is perpendicular to the line.  The line is defined by
    // Dot(N,(x,y)-V0) = 0.  The left side of the line is Dot(N,(x,y)-V0) > 0
    // and the right side of the line is Dot(N,(x,y)-V0) < 0.  We want to know
    // if P is on the right side, so the test for this is
    //   0 > Dot(N,P-V0) = DotPerp(V1-V0,P-V0)
    // where DotPerp((x0,y0),(x1,y1)) = x0*y1 - x1*y0.
    //
    // Numerical problems can occur when this test is used for searching a
    // triangle mesh looking for a triangle that contains P.  If you are
    // currently searching a triangle T0 = <V0,V1,V2>, P is contained in the
    // triangle if it is on the left all three edges, possibly being exactly
    // on an edge.  Equivalently, P is not on the right of any edge.  If you
    // find that P is on the right of an edge,
    //   d0 = DotPerp(V1-V0,P-V0) < 0
    // and that edge is shared by another triangle, T1 = <V1,V0,V3>, then the
    // search continues with that triangle.  It is possible that you again
    // compare P to the same edge, but the vertices are processed in opposite
    // order (all the triangles are consistently ordered).  The comparison is
    //   d1 = DotPerp(V0-V1,P-V1) < 0
    // With exact real-valued arithmetic, if d0 < 0, it must be the case that
    // d1 >= 0.  However, with floating-point arithmetic, if d0 < 0 and is
    // nearly zero, it is possible that d1 < 0 and is nearly zero, due to
    // round-off errors.  If you were to ignore this possibility, the search
    // routine can have an infinite loop, oscillating between triangles T0
    // and T1.
    //
    // To avoid this problem, PointOutsideEdge compares P to the line
    // min(V0,V1) + t*(max(V0,V1) - min(V0,V1)).  The comparison operator<=
    // is used to determine min(V0,V1) and max(V0,V1).
    //
    // TO DO.
    //   1. How to handle the case when V0 and V1 are nearly the same point?
    //      Practitioners tend to eliminate duplicates or points that are
    //      nearly the same before computing the triangulation.
    //   2. How to handle the case when T0 is needle-like, that is when
    //      V0, V1, V2 are collinear or nearly collinear?

    Vector2<Real> kPDiff, kVDiff;
    Real fDotPerp;

    if ( rkV0 <= rkV1 )
    {
        kPDiff = rkP - rkV0;
        kVDiff = rkV1 - rkV0;
        fDotPerp = kVDiff.DotPerp(kPDiff);
        return fDotPerp < (Real)0.0;
    }
    else
    {
        kPDiff = rkP - rkV1;
        kVDiff = rkV0 - rkV1;
        fDotPerp = kVDiff.DotPerp(kPDiff);
        return fDotPerp > (Real)0.0;
    }
}
//----------------------------------------------------------------------------
template <class Real>
bool Delaunay2<Real>::PointInTriangle (const Vector2<Real>& rkP,
    const Vector2<Real>& rkV0, const Vector2<Real>& rkV1,
    const Vector2<Real>& rkV2)
{
    // assert: <V0,V1,V2> is counterclockwise ordered
    return !PointOutsideEdge(rkP,rkV0,rkV1)
        && !PointOutsideEdge(rkP,rkV1,rkV2)
        && !PointOutsideEdge(rkP,rkV2,rkV0);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// explicit instantiation
//----------------------------------------------------------------------------
// float
template WM3_ITEM
class Delaunay2<float>;

// double
template WM3_ITEM
class Delaunay2<double>;
//----------------------------------------------------------------------------
}
