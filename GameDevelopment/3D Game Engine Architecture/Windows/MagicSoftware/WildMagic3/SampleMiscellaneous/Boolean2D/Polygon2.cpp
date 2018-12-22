// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Polygon2.h"
#include "BspTree2.h"

//----------------------------------------------------------------------------
Polygon2::Polygon2 ()
{
    m_pkTree = 0;
}
//----------------------------------------------------------------------------
Polygon2::Polygon2 (const Polygon2& rkP)
{
    m_pkTree = 0;
    *this = rkP;
}
//----------------------------------------------------------------------------
Polygon2::~Polygon2 ()
{
    delete m_pkTree;
}
//----------------------------------------------------------------------------
Polygon2& Polygon2::operator= (const Polygon2& rkP)
{
    m_kVMap = rkP.m_kVMap;
    m_kVArray = rkP.m_kVArray;
    m_kEMap = rkP.m_kEMap;
    m_kEArray = rkP.m_kEArray;
    delete m_pkTree;
    m_pkTree = ( rkP.m_pkTree ? rkP.m_pkTree->GetCopy() : 0 );
    return *this;
}
//----------------------------------------------------------------------------
int Polygon2::InsertVertex (const Vector2d& rkV)
{
    VIterator pkVIter = m_kVMap.find(rkV);
    if ( pkVIter != m_kVMap.end() )
    {
        // vertex already in map, just return its unique index
        return pkVIter->second;
    }

    // vertex not in map, insert it and assign it a unique index
    int i = (int)m_kVArray.size();
    m_kVMap.insert(make_pair(rkV,i));
    m_kVArray.push_back(rkV);
    return i;
}
//----------------------------------------------------------------------------
int Polygon2::InsertEdge (const Edge2& rkE)
{
    assert( rkE.i0 != rkE.i1 );

    EIterator pkEIter = m_kEMap.find(rkE);
    if ( pkEIter != m_kEMap.end() )
    {
        // edge already in map, just return its unique index
        return pkEIter->second;
    }

    // edge not in map, insert it and assign it a unique index
    int i = (int)m_kEArray.size();
    m_kEMap.insert(make_pair(rkE,i));
    m_kEArray.push_back(rkE);
    return i;
}
//----------------------------------------------------------------------------
void Polygon2::SplitEdge (int iV0, int iV1, int iVMid)
{
    // find the edge in the map to get the edge-array index
    EIterator pkEIter = m_kEMap.find(Edge2(iV0,iV1));
    assert ( pkEIter != m_kEMap.end() );
    int iEIndex = pkEIter->second;

    // delete <V0,V1>
    m_kEMap.erase(pkEIter);

    // insert <V0,VM>
    m_kEArray[iEIndex].i1 = iVMid;
    m_kEMap.insert(make_pair(m_kEArray[iEIndex],iEIndex));

    // insert <VM,V1>
    InsertEdge(Edge2(iVMid,iV1));
}
//----------------------------------------------------------------------------
void Polygon2::Finalize ()
{
    delete m_pkTree;
    m_pkTree = new BspTree2(*this,m_kEArray);
}
//----------------------------------------------------------------------------
int Polygon2::GetVertexQuantity () const
{
    return (int)m_kVMap.size();
}
//----------------------------------------------------------------------------
bool Polygon2::GetVertex (int i, Vector2d& rkV) const
{
    if ( 0 <= i && i < (int)m_kVArray.size() )
    {
        rkV = m_kVArray[i];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
int Polygon2::GetEdgeQuantity () const
{
    return (int)m_kEMap.size();
}
//----------------------------------------------------------------------------
bool Polygon2::GetEdge (int i, Edge2& rkE) const
{
    if ( 0 <= i && i < (int)m_kEArray.size() )
    {
        rkE = m_kEArray[i];
        return true;
    }
    return false;
}
//----------------------------------------------------------------------------
void Polygon2::GetInsideEdgesFrom (const Polygon2& rkP, Polygon2& rkInside)
    const
{
    assert( m_pkTree );

    Polygon2 kIgnore;
    for (int i = 0; i < rkP.GetEdgeQuantity(); i++)
    {
        int iV0 = rkP.m_kEArray[i].i0;
        int iV1 = rkP.m_kEArray[i].i1;
        Vector2d kV0 = rkP.m_kVArray[iV0];
        Vector2d kV1 = rkP.m_kVArray[iV1];
        m_pkTree->GetPartition(*this,kV0,kV1,kIgnore,rkInside,rkInside,
            kIgnore);
    }
}
//----------------------------------------------------------------------------
Polygon2 Polygon2::operator~ () const
{
    assert( m_pkTree );

    // negation
    Polygon2 kNeg;
    kNeg.m_kVMap = m_kVMap;
    kNeg.m_kVArray = m_kVArray;
    for (ECIterator pkE = m_kEMap.begin(); pkE != m_kEMap.end(); pkE++)
        kNeg.InsertEdge(Edge2(pkE->first.i1,pkE->first.i0));

    kNeg.m_pkTree = m_pkTree->GetCopy();
    kNeg.m_pkTree->Negate();
    return kNeg;
}
//----------------------------------------------------------------------------
Polygon2 Polygon2::operator& (const Polygon2& rkP) const
{
    assert( m_pkTree );

    // intersection
    Polygon2 kIntersect;
    GetInsideEdgesFrom(rkP,kIntersect);
    rkP.GetInsideEdgesFrom(*this,kIntersect);
    kIntersect.Finalize();
    return kIntersect;
}
//----------------------------------------------------------------------------
Polygon2 Polygon2::operator| (const Polygon2& rkP) const
{
    // union
    const Polygon2& rkQ = *this;
    return ~(~rkQ & ~rkP);
}
//----------------------------------------------------------------------------
Polygon2 Polygon2::operator- (const Polygon2& rkP) const
{
    // difference
    const Polygon2& rkQ = *this;
    return rkQ & ~rkP;
}
//----------------------------------------------------------------------------
Polygon2 Polygon2::operator^ (const Polygon2& rkP) const
{
    // exclusive or
    const Polygon2& rkQ = *this;
    return (rkQ - rkP) | (rkP - rkQ);
}
//----------------------------------------------------------------------------
int Polygon2::PointLocation (const Vector2d& rkV) const
{
    assert( m_pkTree );
    return m_pkTree->PointLocation(*this,rkV);
}
//----------------------------------------------------------------------------
void Polygon2::Print (const char* acFilename) const
{
    ofstream kOStr(acFilename);

    kOStr << "vquantity = " << (int)m_kVArray.size() << endl;
    int i;
    for (i = 0; i < (int)m_kVArray.size(); i++)
    {
        kOStr << i << "  (" << m_kVArray[i].X() << ',' << m_kVArray[i].Y()
            << ')' << endl;
    }
    kOStr << endl;

    kOStr << "equantity = " << (int)m_kEArray.size() << endl;
    for (i = 0; i < (int)m_kEArray.size(); i++)
    {
        kOStr << "  <" << m_kEArray[i].i0 << ',' << m_kEArray[i].i1
            << '>' << endl;
    }
    kOStr << endl;

    kOStr << "bsp tree" << endl;
    if ( m_pkTree )
        m_pkTree->Print(kOStr,0,'r');
    kOStr << endl;
}
//----------------------------------------------------------------------------
