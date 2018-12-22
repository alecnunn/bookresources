// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3TerrainPage.h"
#include "Wm3StandardMesh.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,TerrainPage,TriMesh);
WM3_IMPLEMENT_ABSTRACT_STREAM(TerrainPage);
WM3_IMPLEMENT_DEFAULT_NAME_ID(TerrainPage,TriMesh);

//----------------------------------------------------------------------------
TerrainPage::TerrainPage (int iSize, unsigned short* ausHeight,
    const Vector2f& rkOrigin, float fMinElevation, float fMaxElevation,
    float fSpacing, float fUVBias)
    :
    m_kOrigin(rkOrigin)
{
    // iSize = 2^p + 1, p <= 7
    assert( iSize ==  3 || iSize ==  5 || iSize ==   9 || iSize == 17
        ||  iSize == 33 || iSize == 65 || iSize == 129 );

    // native data
    m_iSize = iSize;
    m_ausHeight = ausHeight;
    m_fMinElevation = fMinElevation;
    m_fMaxElevation = fMaxElevation;
    m_fSpacing = fSpacing;

    InitializeDerivedData();

    // create mesh
    StandardMesh kSM;
    kSM.UVs(true,false);
    float fExtent = m_fSpacing*m_iSizeM1;
    TriMesh* pkMesh = kSM.Rectangle(m_iSize,m_iSize,fExtent,fExtent);
    Vertices = pkMesh->Vertices;
    Indices = pkMesh->Indices;
    m_spkUVs = kSM.GetUVs();
    m_fUVBias = fUVBias;
    delete pkMesh;

    // modify the vertices to use the terrain data
    int iVQuantity = Vertices->GetQuantity();
    Vector3f* akVertex = Vertices->GetData();
    for (int i = 0; i < iVQuantity; i++)
    {
        int iX = i % m_iSize;
        int iY = i / m_iSize;
        akVertex[i].X() = GetX(iX);
        akVertex[i].Y() = GetY(iY);
        akVertex[i].Z() = GetHeight(i);
    }

    UpdateMS();
}
//----------------------------------------------------------------------------
TerrainPage::TerrainPage ()
    :
    m_kOrigin(Vector2f::ZERO)
{
    m_iSize = 0;
    m_ausHeight = 0;
    m_fMinElevation = 0.0f;
    m_fMaxElevation = 0.0f;
    m_fSpacing = 0.0f;
    m_iSizeM1 = 0;
    m_fInvSpacing = 0.0f;
    m_fTextureSpacing = 0.0f;
    m_fMultiplier = 0.0f;
}
//----------------------------------------------------------------------------
TerrainPage::~TerrainPage ()
{
    delete[] m_ausHeight;
}
//----------------------------------------------------------------------------
void TerrainPage::InitializeDerivedData ()
{
    m_iSizeM1 = m_iSize - 1;
    m_fInvSpacing = 1.0f/m_fSpacing;
    m_fTextureSpacing = 1.0f/float(m_iSizeM1);
    m_fMultiplier = (m_fMaxElevation - m_fMinElevation)/65535.0f;
}
//----------------------------------------------------------------------------
float TerrainPage::GetHeight (float fX, float fY) const
{
    float fXGrid = (fX - m_kOrigin.X())*m_fInvSpacing;
    if ( fXGrid < 0.0f || fXGrid >= float(m_iSizeM1) )
    {
        // location not in page
        return Mathf::MAX_REAL;
    }

    float fYGrid = (fY - m_kOrigin.Y())*m_fInvSpacing;
    if ( fYGrid < 0.0f || fYGrid >= float(m_iSizeM1) )
    {
        // location not in page
        return Mathf::MAX_REAL;
    }

    float fCol = Mathf::Floor(fXGrid);
    int iCol = (int)fCol;
    float fRow = Mathf::Floor(fYGrid);
    int iRow = (int)fRow;

    int iIndex = iCol + m_iSize*iRow;
    float fDx = fXGrid - fCol;
    float fDy = fYGrid - fRow;
    float fH00, fH10, fH01, fH11, fHeight;

    if ( (iCol & 1) == (iRow & 1) )
    {
        float fDiff = fDx - fDy;
        fH00 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex];
        fH11 = m_fMinElevation + m_fMultiplier *
            m_ausHeight[iIndex+1+m_iSize];
        if ( fDiff > 0.0f )
        {
            fH10 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex+1];
            fHeight = (1.0f-fDiff-fDy)*fH00+fDiff*fH10+fDy*fH11;
        }
        else
        {
            fH01 = m_fMinElevation + m_fMultiplier *
                m_ausHeight[iIndex+m_iSize];
            fHeight = (1.0f+fDiff-fDx)*fH00-fDiff*fH01+fDx*fH11;
        }
    }
    else
    {
        float fSum = fDx + fDy;
        fH10 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex+1];
        fH01 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex+m_iSize];
        if ( fSum <= 1.0f )
        {
            fH00 = m_fMinElevation + m_fMultiplier*m_ausHeight[iIndex];
            fHeight = (1.0f-fSum)*fH00+fDx*fH10+fDy*fH01;
        }
        else
        {
            fH11 = m_fMinElevation + m_fMultiplier *
                m_ausHeight[iIndex+1+m_iSize];
            fHeight = (fSum-1.0f)*fH11+(1.0f-fDy)*fH10+(1.0f-fDx)*fH01;
        }
    }

    return fHeight;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void TerrainPage::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iSize);
    int iVQuantity = (int)(m_iSize*m_iSize);
    m_ausHeight = new unsigned short[iVQuantity];
    rkStream.Read(iVQuantity,m_ausHeight);
    rkStream.Read(m_kOrigin);
    rkStream.Read(m_fMinElevation);
    rkStream.Read(m_fMaxElevation);
    rkStream.Read(m_fSpacing);

    InitializeDerivedData();

    WM3_END_DEBUG_STREAM_LOAD(TerrainPage);
}
//----------------------------------------------------------------------------
void TerrainPage::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool TerrainPage::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void TerrainPage::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iSize);
    int iVQuantity = Vertices->GetQuantity();
    rkStream.Write(iVQuantity,m_ausHeight);
    rkStream.Write(m_kOrigin);
    rkStream.Write(m_fMinElevation);
    rkStream.Write(m_fMaxElevation);
    rkStream.Write(m_fSpacing);

    WM3_END_DEBUG_STREAM_SAVE(TerrainPage);
}
//----------------------------------------------------------------------------
StringTree* TerrainPage::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(7,1);
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("size =",m_iSize));
    pkTree->SetString(2,StringTree::Format("origin =",m_kOrigin));
    pkTree->SetString(3,StringTree::Format("min elevation =",
        m_fMinElevation));
    pkTree->SetString(4,StringTree::Format("max elevation =",
        m_fMaxElevation));
    pkTree->SetString(5,StringTree::Format("spacing =",m_fSpacing));
    pkTree->SetString(6,StringTree::Format("uv bias =",m_fUVBias));

    pkTree->SetChild(0,TriMesh::SaveStrings());
    return pkTree;
}
//----------------------------------------------------------------------------
int TerrainPage::GetMemoryUsed () const
{
    return sizeof(TerrainPage) - sizeof(TriMesh) + TriMesh::GetMemoryUsed() +
        Vertices->GetQuantity()*sizeof(m_ausHeight[0]) ;
}
//----------------------------------------------------------------------------
int TerrainPage::GetDiskUsed () const
{
    return TriMesh::GetDiskUsed() +
        sizeof(m_iSize) +
        Vertices->GetQuantity()*sizeof(m_ausHeight[0]) +
        sizeof(m_kOrigin) +
        sizeof(m_fMinElevation) +
        sizeof(m_fMaxElevation) +
        sizeof(m_fSpacing);
}
//----------------------------------------------------------------------------
