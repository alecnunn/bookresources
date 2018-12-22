// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3ClodMesh.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,ClodMesh,TriMesh);
WM3_IMPLEMENT_STREAM(ClodMesh);
WM3_IMPLEMENT_DEFAULT_NAME_ID(ClodMesh,TriMesh);

//----------------------------------------------------------------------------
ClodMesh::ClodMesh (Vector3fArrayPtr spkVertices, IntArrayPtr spkIndices,
    bool bGenerateNormals, int iRecordQuantity, CollapseRecord* akRecord)
    :
    TriMesh(spkVertices,spkIndices,bGenerateNormals)
{
    assert( iRecordQuantity > 0 && akRecord );

    m_iTargetRecord = 0;
    m_iCurrentRecord = 0;
    m_iRecordQuantity = iRecordQuantity;
    m_akRecord = akRecord;
}
//----------------------------------------------------------------------------
ClodMesh::ClodMesh ()
{
    m_iTargetRecord = 0;
    m_iCurrentRecord = 0;
    m_iRecordQuantity = 0;
    m_akRecord = 0;
}
//----------------------------------------------------------------------------
ClodMesh::~ClodMesh ()
{
    delete[] m_akRecord;
}
//----------------------------------------------------------------------------
void ClodMesh::SelectLevelOfDetail ()
{
    // Get target record.  The virtual function may be overridden by a
    // derived class to obtain a desired automated change in the target.
    int iTargetRecord = GetAutomatedTargetRecord();

    // collapse mesh (if necessary)
    int* aiIndices = Indices->GetData();
    int i, iC;
    while ( m_iCurrentRecord < iTargetRecord )
    {
        m_iCurrentRecord++;

        // replace indices in connectivity array
        CollapseRecord& rkRecord = m_akRecord[m_iCurrentRecord];
        for (i = 0; i < rkRecord.IQuantity; i++)
        {
            iC = rkRecord.Indices[i];
            assert( aiIndices[iC] == rkRecord.VThrow );
            aiIndices[iC] = rkRecord.VKeep;
        }

        // reduce vertex count (vertices are properly ordered)
        Vertices->SetActiveQuantity(rkRecord.VQuantity);

        // reduce triangle count (triangles are properly ordered)
        Indices->SetActiveQuantity(3*rkRecord.TQuantity);
    }

    // expand mesh (if necessary)
    while ( m_iCurrentRecord > iTargetRecord )
    {
        // restore indices in connectivity array
        CollapseRecord& rkRecord = m_akRecord[m_iCurrentRecord];
        for (i = 0; i < rkRecord.IQuantity; i++)
        {
            iC = rkRecord.Indices[i];
            assert( aiIndices[iC] == rkRecord.VKeep );
            aiIndices[iC] = rkRecord.VThrow;
        }

        m_iCurrentRecord--;
        CollapseRecord& rkPrevRecord = m_akRecord[m_iCurrentRecord];

        // increase vertex count (vertices are properly ordered)
        Vertices->SetActiveQuantity(rkPrevRecord.VQuantity);

        // increase triangle count (triangles are properly ordered)
        Indices->SetActiveQuantity(3*rkPrevRecord.TQuantity);
    }
}
//----------------------------------------------------------------------------
void ClodMesh::Draw (Renderer& rkRenderer, bool bNoCull)
{
    SelectLevelOfDetail();
    TriMesh::Draw(rkRenderer,bNoCull);
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void ClodMesh::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iCurrentRecord);
    rkStream.Read(m_iTargetRecord);
    rkStream.Read(m_iRecordQuantity);
    if ( m_iRecordQuantity )
    {
        m_akRecord = new CollapseRecord[m_iRecordQuantity];
        for (int i = 0; i < m_iRecordQuantity; i++)
            m_akRecord[i].Read(rkStream);
    }
    else
    {
        m_akRecord = 0;
    }

    WM3_END_DEBUG_STREAM_LOAD(ClodMesh);
}
//----------------------------------------------------------------------------
void ClodMesh::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool ClodMesh::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void ClodMesh::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iCurrentRecord);
    rkStream.Write(m_iTargetRecord);
    rkStream.Write(m_iRecordQuantity);
    for (int i = 0; i < m_iRecordQuantity; i++)
        m_akRecord[i].Write(rkStream);

    WM3_END_DEBUG_STREAM_SAVE(ClodMesh);
}
//----------------------------------------------------------------------------
StringTree* ClodMesh::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(4,2);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("curr rec =",m_iCurrentRecord));
    pkTree->SetString(2,StringTree::Format("targ rec =",m_iTargetRecord));
    pkTree->SetString(3,StringTree::Format("rec quantity =",
        m_iRecordQuantity));

    // children
    pkTree->SetChild(0,TriMesh::SaveStrings());

    StringTree* pkAttrTree = new StringTree(1,m_iRecordQuantity);
    pkAttrTree->SetString(0,StringTree::Format("records"));
    for (int i = 0; i < m_iRecordQuantity; i++)
    {
        CollapseRecord& rkRecord = m_akRecord[i];

        char acString[1024], acDummy[128];
        sprintf(acString,"keep = %d, ",rkRecord.VKeep);

        sprintf(acDummy,"throw = %d, ",rkRecord.VThrow);
        strcat(acString,acDummy);

        sprintf(acDummy,"vq = %d, ",rkRecord.VQuantity);
        strcat(acString,acDummy);

        sprintf(acDummy,"tq = %d, ",rkRecord.TQuantity);
        strcat(acString,acDummy);

        sprintf(acDummy,"iq = %d",rkRecord.IQuantity);
        strcat(acString,acDummy);

        pkAttrTree->SetChild(i,StringTree::Format(acString,
            rkRecord.IQuantity,rkRecord.Indices));
    }
    pkTree->SetChild(1,pkAttrTree);

    return pkTree;
}
//----------------------------------------------------------------------------
int ClodMesh::GetMemoryUsed () const
{
    int iSize = sizeof(ClodMesh) - sizeof(TriMesh) +
        TriMesh::GetMemoryUsed();

    for (int i = 0; i < m_iRecordQuantity; i++)
        iSize += m_akRecord[i].GetMemoryUsed();

    return iSize;
}
//----------------------------------------------------------------------------
int ClodMesh::GetDiskUsed () const
{
    int iSize = TriMesh::GetDiskUsed() +
        sizeof(m_iCurrentRecord) +
        sizeof(m_iTargetRecord) +
        sizeof(m_iRecordQuantity);

    for (int i = 0; i < m_iRecordQuantity; i++)
        iSize += m_akRecord[i].GetDiskUsed();

    return iSize;
}
//----------------------------------------------------------------------------
