// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "WUnimaterialMesh.h"
using namespace Wm3;

#include <algorithm>
#include <vector>
using namespace std;

//----------------------------------------------------------------------------
WUnimaterialMesh::WUnimaterialMesh ()
{
    m_iVQuantity = 0;
    m_akVertex = NULL;
    m_akNormal = NULL;
    m_iCQuantity = 0;
    m_akColor = NULL;
    m_iTQuantity = 0;
    m_akTexture = NULL;
    m_iFQuantity = 0;
    m_aiFace = NULL;
    m_aiCFace = NULL;
    m_aiTFace = NULL;
}
//----------------------------------------------------------------------------
int& WUnimaterialMesh::VQuantity ()
{
    return m_iVQuantity;
}
//----------------------------------------------------------------------------
Wm3::Vector3f*& WUnimaterialMesh::Vertex ()
{
    return m_akVertex;
}
//----------------------------------------------------------------------------
Wm3::Vector3f*& WUnimaterialMesh::Normal ()
{
    return m_akNormal;
}
//----------------------------------------------------------------------------
int& WUnimaterialMesh::CQuantity ()
{
    return m_iCQuantity;
}
//----------------------------------------------------------------------------
Wm3::ColorRGB*& WUnimaterialMesh::Color ()
{
    return m_akColor;
}
//----------------------------------------------------------------------------
int& WUnimaterialMesh::TQuantity ()
{
    return m_iTQuantity;
}
//----------------------------------------------------------------------------
Wm3::Vector2f*& WUnimaterialMesh::Texture ()
{
    return m_akTexture;
}
//----------------------------------------------------------------------------
int& WUnimaterialMesh::FQuantity ()
{
    return m_iFQuantity;
}
//----------------------------------------------------------------------------
int*& WUnimaterialMesh::Face ()
{
    return m_aiFace;
}
//----------------------------------------------------------------------------
int*& WUnimaterialMesh::CFace ()
{
    return m_aiCFace;
}
//----------------------------------------------------------------------------
int*& WUnimaterialMesh::TFace ()
{
    return m_aiTFace;
}
//----------------------------------------------------------------------------
Wm3::MaterialStatePtr& WUnimaterialMesh::MState()
{
    return m_spkMState;
}
//----------------------------------------------------------------------------
Wm3::TexturePtr& WUnimaterialMesh::TState()
{
    return m_spkTState;
}
//----------------------------------------------------------------------------
void WUnimaterialMesh::DuplicateGeometry ()
{
    // build array of vertex+attribute objects
    vector<VertexAttr>* akVArray = new vector<VertexAttr>[m_iVQuantity];
    int i;
    for (i = 0; i < 3*m_iFQuantity; i++)
    {
        VertexAttr kAttr;

        kAttr.V = m_aiFace[i];

        if ( m_iCQuantity > 0 )
            kAttr.C = m_aiCFace[i];

        if ( m_iTQuantity > 0 )
            kAttr.T = m_aiTFace[i];

        akVArray[m_aiFace[i]].push_back(kAttr);
    }

    // compute total number of vertices needed
    int iNewVQuantity = 0;
    for (i = 0; i < m_iVQuantity; i++)
    {
        // remove duplicate uvw indices
        sort(akVArray[i].begin(),akVArray[i].end());
        vector<VertexAttr>::iterator pkEnd = unique(akVArray[i].begin(),
            akVArray[i].end());
        akVArray[i].erase(pkEnd,akVArray[i].end());
        iNewVQuantity += akVArray[i].size();
    }

    // Wild Magic geometry data
    Vector3f* akNewVertex = new Vector3f[iNewVQuantity];
    Vector3f* akNewNormal = new Vector3f[iNewVQuantity];

    Wm3::ColorRGB* akNewColor = NULL;
    if ( m_iCQuantity > 0 )
        akNewColor = new Wm3::ColorRGB[iNewVQuantity];

    Vector2f* akNewTexture = NULL;
    if ( m_iTQuantity > 0 )
        akNewTexture = new Vector2f[iNewVQuantity];

    int j, k;
    for (i = 0, k = 0; i < m_iVQuantity; i++)
    {
        vector<VertexAttr>& rkVArray = akVArray[i];
        int iSize = rkVArray.size();
        for (j = 0; j < iSize; j++, k++)
        {
            akNewVertex[k] = m_akVertex[i];
            akNewNormal[k] = m_akNormal[i];

            VertexAttr kAttr = rkVArray[j];

            if ( akNewColor )
                akNewColor[k] = m_akColor[kAttr.C];

            if ( akNewTexture )
                akNewTexture[k] = m_akTexture[kAttr.T];

            // save duplicated attributes for later use
            kAttr.V = k;
            rkVArray.push_back(kAttr);
        }
    }

    // modify the connectivity array to reflect the duplication of vertices
    for (i = 0; i < m_iFQuantity; i++)
    {
        int iThreeI = 3*i;
        int* aiVIndex = m_aiFace+iThreeI;
        int* aiCIndex = ( m_iCQuantity > 0 ? m_aiCFace+iThreeI : NULL );
        int* aiTIndex = ( m_iTQuantity > 0 ? m_aiTFace+iThreeI : NULL );
        for (j = 0; j < 3; j++)
        {
            VertexAttr kAttr;
            kAttr.V = aiVIndex[j];

            if ( aiCIndex )
                kAttr.C = aiCIndex[j];

            if ( aiTIndex )
                kAttr.T = aiTIndex[j];

            // VArray has N original vertices and N duplicates
            vector<VertexAttr>& rkVArray = akVArray[aiVIndex[j]];
            int iHalfSize = rkVArray.size()/2;
            for (k = 0; k < iHalfSize; k++)
            {
                if ( rkVArray[k] == kAttr )
                {
                    // found correct (vert,color,uv) pair, update vert index
                    aiVIndex[j] = rkVArray[iHalfSize+k].V;
                    break;
                }
            }
        }
    }

    delete[] m_akVertex;
    delete[] m_akNormal;
    delete[] m_akColor;
    delete[] m_akTexture;
    delete[] m_aiTFace;

    m_iVQuantity = iNewVQuantity;
    m_akVertex = akNewVertex;
    m_akNormal = akNewNormal;
    m_akColor = akNewColor;
    m_akTexture = akNewTexture;

    delete[] akVArray;
}
//----------------------------------------------------------------------------
TriMesh* WUnimaterialMesh::ToTriMesh()
{
    // triangle mesh from vertices and indices
    Vector3fArray* pkVertices = new Vector3fArray(m_iVQuantity,m_akVertex);
    IntArray* pkIndices = new IntArray(3*m_iFQuantity,m_aiFace);
    TriMesh* pkMesh = new TriMesh(pkVertices,pkIndices,false);

    // triangle normals
    if ( m_akNormal )
        pkMesh->Normals = new Vector3fArray(m_iVQuantity,m_akNormal);

    // materials
    if( m_spkMState )
        pkMesh->SetGlobalState(m_spkMState);

    Wm3::Effect* pkEffect = NULL;

    // vertex colors
    if ( m_akColor )
    {
        pkEffect = new Wm3::Effect;
        pkEffect->ColorRGBs = new ColorRGBArray(m_iVQuantity,m_akColor);
    }

    // texture coordinates
    if ( m_spkTState )
    {
        // TO DO.  How to handle multitexture when splitting?
        if ( !pkEffect )
            pkEffect = new Wm3::Effect;

        Vector2fArray* pkUVs = new Vector2fArray(m_iVQuantity,m_akTexture);
        pkEffect->UVs.Append(pkUVs);
        pkEffect->Textures.Append(m_spkTState);
    }
    else
    {
        assert( !m_akTexture );
        if ( m_akTexture )
            delete[] m_akTexture;
    }

    if ( pkEffect )
        pkMesh->SetEffect(pkEffect);

    return pkMesh;
}
//----------------------------------------------------------------------------
WUnimaterialMesh::VertexAttr::VertexAttr ()
{
    V = -1;
    C = -1;
    T = -1;
}
//----------------------------------------------------------------------------
bool WUnimaterialMesh::VertexAttr::operator== (const VertexAttr& rkAttr) const
{
    return V == rkAttr.V && C == rkAttr.C && T == rkAttr.T;
}
//----------------------------------------------------------------------------
bool WUnimaterialMesh::VertexAttr::operator< (const VertexAttr& rkAttr) const
{
    if ( V < rkAttr.V )
        return true;
    if ( V > rkAttr.V )
        return false;

    if ( C < rkAttr.C )
        return true;
    if ( C > rkAttr.C )
        return false;

    return T < rkAttr.T;
}
//----------------------------------------------------------------------------
