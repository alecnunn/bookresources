// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3RectangleSurface.h"
#include "Wm3CullState.h"
#include "Wm3StandardMesh.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,RectangleSurface,TriMesh);
WM3_IMPLEMENT_STREAM(RectangleSurface);
WM3_IMPLEMENT_DEFAULT_NAME_ID(RectangleSurface,TriMesh);

//----------------------------------------------------------------------------
RectangleSurface::RectangleSurface (ParametricSurfacef* pkSurface,
    int iUSamples, int iVSamples, bool bWantNormals, bool bWantColors,
    bool bDoubleSided, const Vector2f* pkUVMin, const Vector2f* pkUVMax)
{
    assert( pkSurface && pkSurface->IsRectangular() );

    m_pkSurface = pkSurface;
    m_iUSamples = iUSamples;
    m_iVSamples = iVSamples;

    float fUMin = m_pkSurface->GetUMin();
    float fURan = m_pkSurface->GetUMax() - fUMin;
    float fUDelta = fURan/(float)(m_iUSamples-1);
    float fVMin = m_pkSurface->GetVMin();
    float fVRan = m_pkSurface->GetVMax() - fVMin;
    float fVDelta = fVRan/(float)(m_iVSamples-1);

    // compute the vertices, normals, and texture coordinates
    int iVQuantity = m_iUSamples*m_iVSamples;
    Vector3f* akVertex = new Vector3f[iVQuantity];
    Vector3f* akNormal = ( bWantNormals ? new Vector3f[iVQuantity] : 0 );

    Vector2f* akUV = 0;
    float fTUDelta = 0.0f, fTVDelta = 0.0f;
    if ( pkUVMin && pkUVMax )
    {
        akUV = new Vector2f[iVQuantity];
        fTUDelta = (pkUVMax->X() - pkUVMin->X())/fURan;
        fTVDelta = (pkUVMax->Y() - pkUVMin->Y())/fVRan;
    }

    int iU, iV, i;
    for (iU = 0, i = 0; iU < m_iUSamples; iU++)
    {
        float fUIncr = fUDelta*iU;
        float fU = fUMin + fUIncr;
        for (iV = 0; iV < m_iVSamples; iV++)
        {
            float fVIncr = fVDelta*iV;
            float fV = fVMin + fVIncr;
            akVertex[i] = m_pkSurface->P(fU,fV);

            if ( akNormal )
            {
                Vector3f kPos, kTan0, kTan1;
                m_pkSurface->GetFrame(fU,fV,kPos,kTan0,kTan1,akNormal[i]);
            }

            if ( akUV )
            {
                akUV[i].X() = pkUVMin->X() + fTUDelta*fUIncr;
                akUV[i].Y() = pkUVMin->Y() + fTVDelta*fVIncr;
            }
            i++;
        }
    }

    Vertices = new Vector3fArray(iVQuantity,akVertex);
    if ( akNormal )
        Normals = new Vector3fArray(iVQuantity,akNormal);

    // compute the surface triangle connectivity
    int iTQuantity = 2*(m_iUSamples-1)*(m_iVSamples-1);
    int iIQuantity = 3*iTQuantity;
    int* aiIndex = new int[iIQuantity];
    int* aiLocalIndex = aiIndex;
    for (iU = 0, i = 0; iU < m_iUSamples-1; iU++)
    {
        int i0 = i;
        int i1 = i0 + 1;
        i += m_iVSamples;
        int i2 = i;
        int i3 = i2 + 1;
        for (iV = 0; iV < m_iVSamples-1; iV++, aiLocalIndex += 6)
        {
            aiLocalIndex[0] = i0;
            aiLocalIndex[1] = i1;
            aiLocalIndex[2] = i2;
            aiLocalIndex[3] = i1;
            aiLocalIndex[4] = i3;
            aiLocalIndex[5] = i2;
            i0++;
            i1++;
            i2++;
            i3++;
        }
    }
    Indices = new IntArray(iIQuantity,aiIndex);

    if ( bDoubleSided )
    {
        // disable culling
        CullState* pkCS = new CullState;
        pkCS->Enabled = false;
        SetGlobalState(pkCS);
    }

    // create the effect
    if ( bWantColors || akUV )
    {
        Effect* pkEffect = new Effect;

        if ( bWantColors )
        {
            ColorRGB* akColor = new ColorRGB[iVQuantity];
            memset(akColor,0,iVQuantity*sizeof(ColorRGB));
            pkEffect->ColorRGBs = new ColorRGBArray(iVQuantity,akColor);
        }

        if ( akUV )
        {
            Vector2fArray* pkUVs = new Vector2fArray(iVQuantity,akUV);
            pkEffect->UVs.Append(pkUVs);
        }

        SetEffect(pkEffect);
    }

    UpdateModelBound();
}
//----------------------------------------------------------------------------
RectangleSurface::RectangleSurface ()
{
    m_pkSurface = 0;
}
//----------------------------------------------------------------------------
RectangleSurface::~RectangleSurface ()
{
    delete m_pkSurface;
}
//----------------------------------------------------------------------------
void RectangleSurface::UpdateSurface ()
{
    float fUMin = m_pkSurface->GetUMin();
    float fUDelta = (m_pkSurface->GetUMax() - fUMin)/(float)(m_iUSamples-1);
    float fVMin = m_pkSurface->GetVMin();
    float fVDelta = (m_pkSurface->GetVMax() - fVMin)/(float)(m_iVSamples-1);

    Vector3f* akVertex = Vertices->GetData();
    for (int iU = 0, i = 0; iU < m_iUSamples; iU++)
    {
        float fU = fUMin + fUDelta*iU;
        for (int iV = 0; iV < m_iVSamples; iV++)
        {
            float fV = fVMin + fVDelta*iV;
            akVertex[i] = m_pkSurface->P(fU,fV);

            Vector3f* akNormal = ( Normals ? Normals->GetData() : 0 );
            if ( akNormal )
            {
                Vector3f kPos, kTan0, kTan1;
                m_pkSurface->GetFrame(fU,fV,kPos,kTan0,kTan1,akNormal[i]);
            }

            i++;
        }
    }

    UpdateModelBound();
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void RectangleSurface::Load (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iUSamples);
    rkStream.Read(m_iVSamples);

    // TO DO.  See note in RectangleSurface::Save.
    m_pkSurface = 0;
}
//----------------------------------------------------------------------------
void RectangleSurface::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool RectangleSurface::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void RectangleSurface::Save (Stream& rkStream) const
{
    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iUSamples);
    rkStream.Write(m_iVSamples);

    // TO DO.  The class ParametricSurface3 is abstract and does not know
    // about the data representation for the derived-class object that is
    // passed to the RectangleSurface constructor.  Because of this, any
    // loaded RectangleSurface object will require the application to
    // manually set the surface via the Surface() member function.
    //
    // Streaming support should probably be added to the surface classes.
}
//----------------------------------------------------------------------------
StringTree* RectangleSurface::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(3,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("u samples =",m_iUSamples));
    pkTree->SetString(2,StringTree::Format("v samples =",m_iVSamples));

    // children
    pkTree->SetChild(0,TriMesh::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int RectangleSurface::GetMemoryUsed () const
{
    // TO DO.  Does not count the size of the Surface object.  The surface
    // class is virtual, so that system would need to provide a GetBytesUsed
    // member function.
    return sizeof(RectangleSurface) - sizeof(TriMesh) +
        TriMesh::GetMemoryUsed();
}
//----------------------------------------------------------------------------
int RectangleSurface::GetDiskUsed () const
{
    return TriMesh::GetDiskUsed() +
        sizeof(m_iUSamples) +
        sizeof(m_iVSamples);
}
//----------------------------------------------------------------------------
