// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3BoxSurface.h"
#include "Wm3CullState.h"
#include "Wm3ZBufferState.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,BoxSurface,Node);
WM3_IMPLEMENT_STREAM(BoxSurface);
WM3_IMPLEMENT_DEFAULT_NAME_ID(BoxSurface,Node);

//----------------------------------------------------------------------------
BoxSurface::BoxSurface (BSplineVolumef* pkVolume, int iUSamples,
    int iVSamples, int iWSamples, bool bWantNormals, bool bWantColors,
    Effect* pkEffect)
{
    assert( pkVolume );

    m_pkVolume = pkVolume;
    m_iUSamples = iUSamples;
    m_iVSamples = iVSamples;
    m_iWSamples = iWSamples;

    int aiPerm[3];
    TriMesh* pkMesh;

    // u faces
    aiPerm[0] = 1;  aiPerm[1] = 2;  aiPerm[2] = 0;

    // u = 0
    pkMesh = CreateFace(m_iWSamples,m_iVSamples,bWantNormals,bWantColors,
        pkEffect,false,0.0f,aiPerm);
    pkMesh->SetName("u0");
    AttachChild(pkMesh);

    // u = 1
    pkMesh = CreateFace(m_iWSamples,m_iVSamples,bWantNormals,bWantColors,
        pkEffect,true,1.0f,aiPerm);
    pkMesh->SetName("u1");
    AttachChild(pkMesh);

    // v faces
    aiPerm[0] = 0;  aiPerm[1] = 2;  aiPerm[2] = 1;

    // v = 0
    pkMesh = CreateFace(m_iWSamples,m_iUSamples,bWantNormals,bWantColors,
        pkEffect,true,0.0f,aiPerm);
    pkMesh->SetName("v0");
    AttachChild(pkMesh);

    // v = 1
    pkMesh = CreateFace(m_iWSamples,m_iUSamples,bWantNormals,bWantColors,
        pkEffect,false,1.0f,aiPerm);
    pkMesh->SetName("v1");
    AttachChild(pkMesh);

    // w faces
    aiPerm[0] = 0;  aiPerm[1] = 1;  aiPerm[2] = 2;

    // w = 0
    pkMesh = CreateFace(m_iVSamples,m_iUSamples,bWantNormals,bWantColors,
        pkEffect,false,0.0f,aiPerm);
    pkMesh->SetName("w0");
    AttachChild(pkMesh);

    // w = 1
    pkMesh = CreateFace(m_iVSamples,m_iUSamples,bWantNormals,bWantColors,
        pkEffect,true,1.0f,aiPerm);
    pkMesh->SetName("w1");
    AttachChild(pkMesh);
}
//----------------------------------------------------------------------------
BoxSurface::BoxSurface ()
{
    m_pkVolume = 0;
}
//----------------------------------------------------------------------------
BoxSurface::~BoxSurface ()
{
    delete m_pkVolume;
}
//----------------------------------------------------------------------------
TriMesh* BoxSurface::CreateFace (int iRows, int iCols, bool bWantNormals,
    bool bWantColors, Effect* pkEffect, bool bCCW, float fFaceValue,
    int aiPerm[3])
{
    int iVQuantity = iRows*iCols;
    Vector3f* akVertex = new Vector3f[iVQuantity];
    Vector3fArray* pkVertices = new Vector3fArray(iVQuantity,akVertex);

    Vector3f* akNormal = 0;
    if ( bWantNormals )
        akNormal = new Vector3f[iVQuantity];

    ColorRGB* akColor = 0;
    if ( bWantColors )
    {
        akColor = new ColorRGB[iVQuantity];
        memset(akColor,0,iVQuantity*sizeof(ColorRGB));
    }

    Vector2f* akUV = 0;
    if ( pkEffect )
        akUV = new Vector2f[iVQuantity];

    float afP[3];
    afP[aiPerm[2]] = fFaceValue;
    float fSign = ( bCCW ? 1.0f : -1.0f );
    float fRFactor = 1.0f/(float)(iRows-1);
    float fCFactor = 1.0f/(float)(iCols-1);
    int iRow, iCol, i;
    for (iRow = 0, i = 0; iRow < iRows; iRow++)
    {
        afP[aiPerm[1]] = iRow*fRFactor;
        for (iCol = 0; iCol < iCols; iCol++)
        {
            afP[aiPerm[0]] = iCol*fCFactor;
            akVertex[i] = m_pkVolume->GetPosition(afP);

            if ( akNormal )
            {
                Vector3f kDC = m_pkVolume->GetDerivative(aiPerm[0],afP);
                Vector3f kDR = m_pkVolume->GetDerivative(aiPerm[1],afP);
                akNormal[i] = fSign*kDC.UnitCross(kDR);
            }

            if ( akUV )
                akUV[i] = Vector2f(afP[aiPerm[0]],afP[aiPerm[1]]);

            i++;
        }
    }

    int iTQuantity = 2*(iRows-1)*(iCols-1);
    int* aiIndex = new int[3*iTQuantity];
    int* aiLocalIndex = aiIndex;
    for (iRow = 0, i = 0; iRow < iRows-1; iRow++)
    {
        int i0 = i;
        int i1 = i0 + 1;
        i += iCols;
        int i2 = i;
        int i3 = i2 + 1;
        for (iCol = 0; iCol < iCols-1; iCol++, aiLocalIndex += 6)
        {
            if ( bCCW )
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i1;
                aiLocalIndex[2] = i2;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i3++;
                aiLocalIndex[5] = i2++;
            }
            else
            {
                aiLocalIndex[0] = i0++;
                aiLocalIndex[1] = i2;
                aiLocalIndex[2] = i1;
                aiLocalIndex[3] = i1++;
                aiLocalIndex[4] = i2++;
                aiLocalIndex[5] = i3++;
            }
        }
    }
    IntArray* pkIndices = new IntArray(3*iTQuantity,aiIndex);

    TriMesh* pkMesh = new TriMesh(pkVertices,pkIndices,false);

    if ( akNormal )
        pkMesh->Normals = new Vector3fArray(iVQuantity,akNormal);

    if ( akColor || akUV )
    {
        Effect* pkFaceEffect = ( pkEffect ? pkEffect : new Effect );

        if ( akColor )
            pkFaceEffect->ColorRGBs = new ColorRGBArray(iVQuantity,akColor);

        if ( akUV )
            pkFaceEffect->UVs.Append(new Vector2fArray(iVQuantity,akUV));

        pkMesh->SetEffect(pkFaceEffect);
    }

    return pkMesh;
}
//----------------------------------------------------------------------------
void BoxSurface::UpdateFace (int iRows, int iCols, Vector3f* akVertex,
    Vector3f* akNormal, bool bCCW, float fFaceValue, int aiPerm[3])
{
    float afP[3];
    afP[aiPerm[2]] = fFaceValue;
    float fSign = ( bCCW ? 1.0f : -1.0f );
    float fRFactor = 1.0f/(float)(iRows-1);
    float fCFactor = 1.0f/(float)(iCols-1);
    for (int iRow = 0, i = 0; iRow < iRows; iRow++)
    {
        afP[aiPerm[1]] = iRow*fRFactor;
        for (int iCol = 0; iCol < iCols; iCol++)
        {
            afP[aiPerm[0]] = iCol*fCFactor;
            akVertex[i] = m_pkVolume->GetPosition(afP);

            if ( akNormal )
            {
                Vector3f kDC = m_pkVolume->GetDerivative(aiPerm[0],afP);
                Vector3f kDR = m_pkVolume->GetDerivative(aiPerm[1],afP);
                akNormal[i] = fSign*kDC.UnitCross(kDR);
            }

            i++;
        }
    }
}
//----------------------------------------------------------------------------
void BoxSurface::UpdateSurface ()
{
    int aiPerm[3];
    TriMesh* pkMesh;
    Vector3f* akVertex;
    Vector3f* akNormal;

    // u faces
    aiPerm[0] = 1;  aiPerm[1] = 2;  aiPerm[2] = 0;

    // u = 0
    pkMesh = StaticCast<TriMesh>(GetChild(0));
    akVertex = pkMesh->Vertices->GetData();
    akNormal = ( pkMesh->Normals? pkMesh->Normals->GetData() : 0 );
    UpdateFace(m_iWSamples,m_iVSamples,akVertex,akNormal,false,0.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // u = 1
    pkMesh = StaticCast<TriMesh>(GetChild(1));
    akVertex = pkMesh->Vertices->GetData();
    akNormal = ( pkMesh->Normals? pkMesh->Normals->GetData() : 0 );
    UpdateFace(m_iWSamples,m_iVSamples,akVertex,akNormal,true,1.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // v faces
    aiPerm[0] = 0;  aiPerm[1] = 2;  aiPerm[2] = 1;

    // v = 0
    pkMesh = StaticCast<TriMesh>(GetChild(2));
    akVertex = pkMesh->Vertices->GetData();
    akNormal = ( pkMesh->Normals? pkMesh->Normals->GetData() : 0 );
    UpdateFace(m_iWSamples,m_iUSamples,akVertex,akNormal,true,0.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // v = 1
    pkMesh = StaticCast<TriMesh>(GetChild(3));
    akVertex = pkMesh->Vertices->GetData();
    akNormal = ( pkMesh->Normals? pkMesh->Normals->GetData() : 0 );
    UpdateFace(m_iWSamples,m_iUSamples,akVertex,akNormal,false,1.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // w faces
    aiPerm[0] = 0;  aiPerm[1] = 1;  aiPerm[2] = 2;

    // w = 0
    pkMesh = StaticCast<TriMesh>(GetChild(4));
    akVertex = pkMesh->Vertices->GetData();
    akNormal = ( pkMesh->Normals? pkMesh->Normals->GetData() : 0 );
    UpdateFace(m_iVSamples,m_iUSamples,akVertex,akNormal,false,0.0f,aiPerm);
    pkMesh->UpdateMS(false);

    // w = 1
    pkMesh = StaticCast<TriMesh>(GetChild(5));
    akVertex = pkMesh->Vertices->GetData();
    akNormal = ( pkMesh->Normals? pkMesh->Normals->GetData() : 0 );
    UpdateFace(m_iVSamples,m_iUSamples,akVertex,akNormal,true,1.0f,aiPerm);
    pkMesh->UpdateMS(false);
}
//----------------------------------------------------------------------------
void BoxSurface::EnableSorting ()
{
    // no culling (all faces drawn)
    CullState* pkCS = new CullState;
    pkCS->Enabled = false;
    SetGlobalState(pkCS);

    // write the zbuffer, but do not read (since we are manually sorting)
    ZBufferState* pkZS = new ZBufferState;
    pkZS->Enabled = false;
    pkZS->Writable = true;
    pkZS->Compare = ZBufferState::CF_LEQUAL;
    SetGlobalState(pkZS);

    UpdateRS();
    m_bDoSort = true;
}
//----------------------------------------------------------------------------
void BoxSurface::DisableSorting ()
{
    RemoveGlobalState(GlobalState::CULL);
    RemoveGlobalState(GlobalState::ZBUFFER);
    UpdateRS();
    m_bDoSort = false;
}
//----------------------------------------------------------------------------
void BoxSurface::SortFaces (const Vector3f& rkWorldViewDirection)
{
    if ( !m_bDoSort )
    {
        // sorting must be enabled in order to sort...
        return;
    }

    // Inverse transform the world view direction into the local space of
    // the box.
    Vector3f kLocalDir = World.InvertVector(rkWorldViewDirection);

    // Store back-facing faces in the front of the array.  Store front-facing
    // faces in the rear of the array.
    SpatialPtr aspkSort[6];
    int iBackFace = 0, iFrontFace = 5;
    int i;
    for (i = 0; i < 6; i++)
    {
        const char* acName = GetChild(i)->GetName();
        if ( acName[0] == 'u' )
        {
            if ( acName[1] == '0' )
            {
                if ( kLocalDir.X() > 0.0f )
                    aspkSort[iFrontFace--] = GetChild(i);
                else
                    aspkSort[iBackFace++] = GetChild(i);
            }
            else  // acName[1] == '1'
            {
                if ( kLocalDir.X() < 0.0f )
                    aspkSort[iFrontFace--] = GetChild(i);
                else
                    aspkSort[iBackFace++] = GetChild(i);
            }
        }
        else if ( acName[0] == 'v' )
        {
            if ( acName[1] == '0' )
            {
                if ( kLocalDir.Y() > 0.0f )
                    aspkSort[iFrontFace--] = GetChild(i);
                else
                    aspkSort[iBackFace++] = GetChild(i);
            }
            else  // acName[1] == '1'
            {
                if ( kLocalDir.Y() < 0.0f )
                    aspkSort[iFrontFace--] = GetChild(i);
                else
                    aspkSort[iBackFace++] = GetChild(i);
            }
        }
        else // acName[0] == 'w'
        {
            if ( acName[1] == '0' )
            {
                if ( kLocalDir.Z() > 0.0f )
                    aspkSort[iFrontFace--] = GetChild(i);
                else
                    aspkSort[iBackFace++] = GetChild(i);
            }
            else  // acName[1] == '1'
            {
                if ( kLocalDir.Z() < 0.0f )
                    aspkSort[iFrontFace--] = GetChild(i);
                else
                    aspkSort[iBackFace++] = GetChild(i);
            }
        }
        DetachChildAt(i);
    }
    assert( iBackFace - iFrontFace == 1 );

    // reassign the sorted children to the node parent
    for (i = 0; i < 6; i++)
        SetChild(i,aspkSort[i]);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void BoxSurface::Load (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iUSamples);
    rkStream.Read(m_iVSamples);
    rkStream.Read(m_iWSamples);
    rkStream.Read(m_bDoSort);

    // TO DO.  See note in BoxSurface::Save.
    m_pkVolume = 0;
}
//----------------------------------------------------------------------------
void BoxSurface::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Node::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool BoxSurface::Register (Stream& rkStream) const
{
    return Node::Register(rkStream);
}
//----------------------------------------------------------------------------
void BoxSurface::Save (Stream& rkStream) const
{
    Node::Save(rkStream);

    // native data
    rkStream.Write(m_iUSamples);
    rkStream.Write(m_iVSamples);
    rkStream.Write(m_iWSamples);
    rkStream.Write(m_bDoSort);

    // TO DO.  The class BSplineVolume is abstract and does not know
    // about the data representation for the derived-class object that is
    // passed to the BoxSurface constructor.  Because of this, any
    // loaded BoxSurface object will require the application to
    // manually set the volume via the Volume() member function.
}
//----------------------------------------------------------------------------
StringTree* BoxSurface::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(5,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("u samples =",m_iUSamples));
    pkTree->SetString(2,StringTree::Format("v samples =",m_iVSamples));
    pkTree->SetString(3,StringTree::Format("w samples =",m_iWSamples));
    pkTree->SetString(4,StringTree::Format("sorting enabled =",m_bDoSort));

    // children
    pkTree->SetChild(0,Node::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int BoxSurface::GetMemoryUsed () const
{
    // TO DO.  Does not count the size of the volume object.  The volume
    // class is virtual, so that system would need to provide a GetBytesUsed
    // member function.
    int iBaseSize = sizeof(BoxSurface) - sizeof(Node);
    int iTotalSize = iBaseSize + Node::GetMemoryUsed();
    return iTotalSize;
}
//----------------------------------------------------------------------------
int BoxSurface::GetDiskUsed () const
{
    return Node::GetDiskUsed() +
        sizeof(m_iUSamples) +
        sizeof(m_iVSamples) +
        sizeof(m_iWSamples) +
        sizeof(m_bDoSort);
}
//----------------------------------------------------------------------------
