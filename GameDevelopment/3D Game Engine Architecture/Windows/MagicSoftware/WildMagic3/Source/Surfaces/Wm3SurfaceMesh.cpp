// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3SurfaceMesh.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,SurfaceMesh,TriMesh);
WM3_IMPLEMENT_STREAM(SurfaceMesh);

//----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh (Vector3fArrayPtr spkVertices,
    IntArrayPtr spkIndices, bool bGenerateNormals, Vector2fArrayPtr spkParams,
    SurfacePatchPtr* aspkPatch, bool bAllowAttributes,
    bool bAllowDynamicChange)
    :
    TriMesh(spkVertices,spkIndices,bGenerateNormals)
{
    assert( spkVertices && spkVertices->GetData() );
    assert( spkIndices && spkIndices->GetData() );
    assert( spkParams && spkParams->GetData() );
    assert( aspkPatch );

    m_spkOrigVertices = spkVertices;
    m_spkOrigIndices = spkIndices;
    m_spkOrigParams = spkParams;
    m_aspkPatch = aspkPatch;
    m_iLevel = 0;
    m_iFullVQuantity = m_spkOrigVertices->GetQuantity();
    m_iPatchQuantity = m_spkOrigIndices->GetQuantity()/3;

    m_bAllowAttributes = bAllowAttributes;
    if ( m_bAllowAttributes )
        m_akVTree = new VertexTree[m_iFullVQuantity];
    else
        m_akVTree = 0;

    m_bAllowDynamicChange = bAllowDynamicChange;
    if ( m_bAllowDynamicChange )
    {
        m_akSInfo = new SurfaceInfo[m_iFullVQuantity];
        InitializeSurfaceInfo();
    }
    else
    {
        m_akSInfo = 0;
    }
}
//----------------------------------------------------------------------------
SurfaceMesh::SurfaceMesh ()
{
    m_aspkPatch = 0;
    m_iLevel = 0;
    m_iFullVQuantity = 0;
    m_iPatchQuantity = 0;
    m_akVTree = 0;
    m_bAllowAttributes = false;
    m_bAllowDynamicChange = false;
    m_akSInfo = 0;
}
//----------------------------------------------------------------------------
SurfaceMesh::~SurfaceMesh ()
{
    delete[] m_aspkPatch;
    delete[] m_akVTree;
    delete[] m_akSInfo;
}
//----------------------------------------------------------------------------
void SurfaceMesh::SetLevel (int iLevel)
{
    if ( !m_spkOrigVertices )
    {
        // the mesh is locked, no subdivision allowed
        return;
    }

    assert( iLevel >= 0 );
    if ( iLevel == m_iLevel )
        return;
    m_iLevel = iLevel;

    // allocate the storage for subdivision
    int iVQTotal, iEQTotal, iTQTotal;
    Vector3f* akVertex;
    THashSet<Edge>* pkESet;
    Triangle* akTriangle;
    Allocate(iVQTotal,akVertex,iEQTotal,pkESet,iTQTotal,akTriangle);

    // subdivide the mesh
    int iVQ = m_spkOrigVertices->GetQuantity();
    int iEQ = pkESet->GetQuantity();
    int iTQ = m_spkOrigIndices->GetQuantity()/3;
    int i;
    for (i = 0; i < m_iLevel; i++)
    {
        int iVQNext = iVQ + iEQ;
        int iEQNext = 2*iEQ + 3*iTQ;
        int iTQNext = 4*iTQ;
        Subdivide(iVQ,akVertex,iEQ,pkESet,iTQ,akTriangle);
        assert( iVQ == iVQNext && iEQ == iEQNext && iTQ == iTQNext );
        iVQ = iVQNext;
        iEQ = iEQNext;
        iTQ = iTQNext;
    }

    // check to make sure the implementation is correct
    assert( iVQ == iVQTotal && iEQ == iEQTotal && iTQ == iTQTotal );
    m_iFullVQuantity = iVQTotal;

    // create the triangle mesh for the subdivision
    Vertices = new Vector3fArray(iVQTotal,akVertex);
    UpdateModelBound();

    int iIQTotal = 3*iTQTotal;
    int* aiIndex = new int[iIQTotal];
    int* piIndex = aiIndex;
    for (i = 0; i < iTQTotal; i++)
    {
        Triangle& rkT = akTriangle[i];
        for (int j = 0; j < 3; j++)
            *piIndex++ = rkT.V[j];
    }
    Indices = new IntArray(iIQTotal,aiIndex);

    delete pkESet;
    delete[] akTriangle;

    OnDynamicChange();
}
//----------------------------------------------------------------------------
void SurfaceMesh::Allocate (int& riVQTotal, Vector3f*& rakVertex,
    int& riEQTotal, THashSet<Edge>*& rpkESet, int& riTQTotal,
    Triangle*& rakTriangle)
{
    // the number of original vertices
    int iVQOrig = m_spkOrigVertices->GetQuantity();

    // the number of original indices and triangles
    int iIQOrig = m_spkOrigIndices->GetQuantity();
    int iTQOrig = iIQOrig/3;

    // count the number of unique edges in the mesh
    const int* piIndex = m_spkOrigIndices->GetData();
    rpkESet = new THashSet<Edge>(iIQOrig);
    rpkESet->UserHashFunction = &Edge::HashFunction;
    int i, iV0, iV1, iV2;
    for (i = 0; i < iTQOrig; i++)
    {
        // get the vertex indices for the triangle
        iV0 = *piIndex++;
        iV1 = *piIndex++;
        iV2 = *piIndex++;

        // store the edges with the vertex indices
        rpkESet->Insert(Edge(iV0,iV1));
        rpkESet->Insert(Edge(iV1,iV2));
        rpkESet->Insert(Edge(iV2,iV0));
    }
    int iEQOrig = rpkESet->GetQuantity();
    delete rpkESet;

    // determine the various quantities needed for subdivision
    riVQTotal = iVQOrig;
    riEQTotal = iEQOrig;
    riTQTotal = iTQOrig;
    for (i = 0; i < m_iLevel; i++)
    {
        riVQTotal = riVQTotal + riEQTotal;
        riEQTotal = 2*riEQTotal + 3*riTQTotal;
        riTQTotal = 4*riTQTotal;
    }

    // allocate storage for the subdivision
    rakVertex = new Vector3f[riVQTotal];
    rpkESet = new THashSet<Edge>(riEQTotal);
    rpkESet->UserHashFunction = &Edge::HashFunction;
    rakTriangle = new Triangle[riTQTotal];
    if ( m_bAllowAttributes )
    {
        delete[] m_akVTree;
        m_akVTree = new VertexTree[riVQTotal];
    }
    if ( m_bAllowDynamicChange )
    {
        delete[] m_akSInfo;
        m_akSInfo = new SurfaceInfo[riVQTotal];
        InitializeSurfaceInfo();
    }

    // copy the original vertices to the storage
    const Vector3f* akOrigVertex = m_spkOrigVertices->GetData();
    memcpy(rakVertex,akOrigVertex,iVQOrig*sizeof(Vector3f));

    // initialize the edge and triangle data structures
    const Vector2f* pkOrigParam = m_spkOrigParams->GetData();
    piIndex = m_spkOrigIndices->GetData();
    for (i = 0; i < iTQOrig; i++)
    {
        Triangle& rkT = rakTriangle[i];

        // surface affecting the triangle
        rkT.Patch = m_aspkPatch[i];

        // get the vertex indices for the triangle
        rkT.V[0] = iV0 = *piIndex++;
        rkT.V[1] = iV1 = *piIndex++;
        rkT.V[2] = iV2 = *piIndex++;

        // get the parameters associated with the vertices
        const Vector2f& rkP0 = *pkOrigParam++;
        const Vector2f& rkP1 = *pkOrigParam++;
        const Vector2f& rkP2 = *pkOrigParam++;

        // store the edges with the surface and parameter values
        InsertInto(rpkESet,rkT.Patch,iV0,iV1,rkP0,rkP1,1);
        InsertInto(rpkESet,rkT.Patch,iV1,iV2,rkP1,rkP2,1);
        InsertInto(rpkESet,rkT.Patch,iV2,iV0,rkP2,rkP0,1);
    }

    assert( rpkESet->GetQuantity() == iEQOrig );
}
//----------------------------------------------------------------------------
void SurfaceMesh::Subdivide (int& riVQ, Vector3f* akVertex, int& riEQ,
    THashSet<Edge>* pkESet, int& riTQ, Triangle* akTriangle)
{
    Edge* pkE = pkESet->GetFirst();
    while ( pkE )
    {
        // Compute the vertex at the average of the parameter values.
        // The index and parameter values of the new vertex will be used
        // when subdividing the triangles.
        pkE->ParamMid = 0.5f*(pkE->Param[0] + pkE->Param[1]);
        akVertex[riVQ] = pkE->Patch->P(pkE->ParamMid.X(),pkE->ParamMid.Y());

        if ( m_bAllowAttributes )
        {
            m_akVTree[riVQ].V[0] = pkE->V[0];
            m_akVTree[riVQ].V[1] = pkE->V[1];
        }

        if ( m_bAllowDynamicChange )
        {
            m_akSInfo[riVQ].Patch = pkE->Patch;
            m_akSInfo[riVQ].Param = pkE->ParamMid;
        }

        pkE->VMid = riVQ++;

        pkE = pkESet->GetNext();
    }

    int i, iMax = riTQ;  // allows riTQ to change inside the loop
    for (i = 0; i < iMax; i++)
    {
        Triangle& rkT0 = akTriangle[i];

        // get the vertices
        int iV0 = rkT0.V[0];
        int iV1 = rkT0.V[1];
        int iV2 = rkT0.V[2];

        // get the edge information
        Edge* pkE01 = pkESet->Get(Edge(iV0,iV1));
        Edge* pkE12 = pkESet->Get(Edge(iV1,iV2));
        Edge* pkE20 = pkESet->Get(Edge(iV2,iV0));
        Vector2f kP0 = pkE01->Param[pkE01->V[0] == iV0 ? 0 : 1];
        Vector2f kP1 = pkE12->Param[pkE12->V[0] == iV1 ? 0 : 1];
        Vector2f kP2 = pkE20->Param[pkE20->V[0] == iV2 ? 0 : 1];

        // get the midpoint information
        int iV01 = pkE01->VMid;
        Vector2f kP01 = pkE01->ParamMid;
        int iV12 = pkE12->VMid;
        Vector2f kP12 = pkE12->ParamMid;
        int iV20 = pkE20->VMid;
        Vector2f kP20 = pkE20->ParamMid;

        // if done with edges, remove them
        if ( --pkE01->References == 0 )
        {
            pkESet->Remove(*pkE01);
        }
        if ( --pkE12->References == 0 )
        {
            pkESet->Remove(*pkE12);
        }
        if ( --pkE20->References == 0 )
        {
            pkESet->Remove(*pkE20);
        }

        // insert the six new boundary edges
        InsertInto(pkESet,rkT0.Patch,iV0,iV01,kP0,kP01,1);
        InsertInto(pkESet,rkT0.Patch,iV01,iV1,kP01,kP1,1);
        InsertInto(pkESet,rkT0.Patch,iV1,iV12,kP1,kP12,1);
        InsertInto(pkESet,rkT0.Patch,iV12,iV2,kP12,kP2,1);
        InsertInto(pkESet,rkT0.Patch,iV2,iV20,kP2,kP20,1);
        InsertInto(pkESet,rkT0.Patch,iV20,iV0,kP20,kP0,1);

        // insert the three new interior edges
        InsertInto(pkESet,rkT0.Patch,iV01,iV12,kP01,kP12,2);
        InsertInto(pkESet,rkT0.Patch,iV12,iV20,kP12,kP20,2);
        InsertInto(pkESet,rkT0.Patch,iV20,iV01,kP20,kP01,2);

        // the old triangle is replaced by the new interior triangle
        rkT0.V[0] = iV01;
        rkT0.V[1] = iV12;
        rkT0.V[2] = iV20;

        // insert the remaining triangles in new slots
        Triangle& rkT1 = akTriangle[riTQ++];
        rkT1.Patch = rkT0.Patch;
        rkT1.V[0] = iV0;
        rkT1.V[1] = iV01;
        rkT1.V[2] = iV20;

        Triangle& rkT2 = akTriangle[riTQ++];
        rkT2.Patch = rkT0.Patch;
        rkT2.V[0] = iV01;
        rkT2.V[1] = iV1;
        rkT2.V[2] = iV12;

        Triangle& rkT3 = akTriangle[riTQ++];
        rkT3.Patch = rkT0.Patch;
        rkT3.V[0] = iV20;
        rkT3.V[1] = iV12;
        rkT3.V[2] = iV2;
    }

    riEQ = pkESet->GetQuantity();
}
//----------------------------------------------------------------------------
void SurfaceMesh::InsertInto (THashSet<Edge>* pkESet,
    SurfacePatchPtr spkPatch, int iV0, int iV1, const Vector2f& rkParam0,
    const Vector2f& rkParam1, int iNewReferences)
{
    Edge* pkE = pkESet->Insert(Edge(iV0,iV1));
    if ( pkE->References == 0 )
    {
        pkE->Patch = spkPatch;
        pkE->Param[0] = rkParam0;
        pkE->Param[1] = rkParam1;
    }
    pkE->References += iNewReferences;
}
//----------------------------------------------------------------------------
float* SurfaceMesh::GetAttributes (int iSize, const float* afAttr) const
{
    if ( !m_bAllowAttributes )
        return 0;

    int iOrigVQuantity = m_spkOrigVertices->GetQuantity();
    float* afFullAttr = new float[iSize*m_iFullVQuantity];

    // use a direct copy for the original vertex attributes
    memcpy(afFullAttr,afAttr,iSize*iOrigVQuantity*sizeof(float));

    // Traverse the vertex tree to compute the remaining attributes.  The
    // order of vertex insertion guarantees that the child attributes are
    // already computed, so a recursive call is not necessary.
    for (int i = iOrigVQuantity; i < m_iFullVQuantity; i++)
    {
        VertexTree& rkNode = m_akVTree[i];
        assert( rkNode.V[0] >= 0 );

        float* afValue = &afFullAttr[iSize*i];
        float* afCValue0 = &afFullAttr[iSize*rkNode.V[0]];
        float* afCValue1 = &afFullAttr[iSize*rkNode.V[1]];
        for (int j = 0; j < iSize; j++)
        {
            // compute the attribute to be the average of attributes
            afValue[j] = 0.5f*(afCValue0[j] + afCValue1[j]);
        }
    }

    return afFullAttr;
}
//----------------------------------------------------------------------------
void SurfaceMesh::OnDynamicChange ()
{
    if ( m_bAllowDynamicChange )
    {
        Vector3f* akVertex = Vertices->GetData();
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            SurfaceInfo& rkSI = m_akSInfo[i];
            akVertex[i] = rkSI.Patch->P(rkSI.Param.X(),rkSI.Param.Y());
        }

        UpdateMS();
    }
}
//----------------------------------------------------------------------------
void SurfaceMesh::Lock ()
{
    if ( m_spkOrigVertices )
    {
        m_spkOrigVertices = 0;
        m_spkOrigIndices = 0;
        m_spkOrigParams = 0;
        delete[] m_aspkPatch;
        m_aspkPatch = 0;
        delete[] m_akVTree;
        m_akVTree = 0;
    }

    // The array m_akSInfo is not deleted.  This allows dynamic updates of
    // the vertices of the mesh, even though the mesh is "locked".
}
//----------------------------------------------------------------------------
void SurfaceMesh::InitializeSurfaceInfo ()
{
    // Because vertices are shared by triangles, the last visited triangle
    // for a vertex donates its patch and parameter values to that vertex.
    const int* piIndex = m_spkOrigIndices->GetData();
    const Vector2f* pkOrigParam = m_spkOrigParams->GetData();
    for (int i = 0; i < m_iPatchQuantity; i++)
    {
        int iV0 = *piIndex++;
        int iV1 = *piIndex++;
        int iV2 = *piIndex++;
        m_akSInfo[iV0].Patch = m_aspkPatch[i];
        m_akSInfo[iV0].Param = *pkOrigParam++;
        m_akSInfo[iV1].Patch = m_aspkPatch[i];
        m_akSInfo[iV1].Param = *pkOrigParam++;
        m_akSInfo[iV2].Patch = m_aspkPatch[i];
        m_akSInfo[iV2].Param = *pkOrigParam++;
    }
}
//----------------------------------------------------------------------------
SurfaceMesh::Edge::Edge (int iV0, int iV1)
{
    Patch = 0;
    V[0] = iV0;
    V[1] = iV1;
    Param[0] = Vector2f::ZERO;
    Param[1] = Vector2f::ZERO;
    VMid = -1;
    ParamMid = Vector2f::ZERO;
    References = 0;
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Edge::operator== (const Edge& rkEdge) const
{
    return (V[0] == rkEdge.V[0] && V[1] == rkEdge.V[1])
        || (V[1] == rkEdge.V[0] && V[0] == rkEdge.V[1]);
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Edge::operator!= (const Edge& rkEdge) const
{
    return !(operator==(rkEdge));
}
//----------------------------------------------------------------------------
int SurfaceMesh::Edge::HashFunction (const Edge& rkEdge)
{
    return rkEdge.V[0] ^ rkEdge.V[1];
}
//----------------------------------------------------------------------------
SurfaceMesh::Triangle::Triangle ()
{
    Patch = 0;
    for (int i = 0; i < 3; i++)
        V[i] = -1;
}
//----------------------------------------------------------------------------
SurfaceMesh::VertexTree::VertexTree ()
{
    V[0] = -1;
    V[1] = -1;
}
//----------------------------------------------------------------------------
SurfaceMesh::SurfaceInfo::SurfaceInfo ()
{
    Patch = 0;
    Param = Vector2f::ZERO;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* SurfaceMesh::GetObjectByName (const String& rkName)
{
    Object* pkFound = TriMesh::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_spkOrigVertices )
    {
        pkFound = m_spkOrigVertices->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( m_spkOrigIndices )
    {
        pkFound = m_spkOrigIndices->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( m_spkOrigParams )
    {
        pkFound = m_spkOrigParams->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    if ( m_aspkPatch )
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if ( m_aspkPatch[i] )
            {
                pkFound = m_aspkPatch[i]->GetObjectByName(rkName);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------
void SurfaceMesh::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    TriMesh::GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkOrigVertices )
        m_spkOrigVertices->GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkOrigIndices )
        m_spkOrigIndices->GetAllObjectsByName(rkName,rkObjects);

    if ( m_spkOrigParams )
        m_spkOrigParams->GetAllObjectsByName(rkName,rkObjects);

    if ( m_aspkPatch )
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if ( m_aspkPatch[i] )
                m_aspkPatch[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }
}
//----------------------------------------------------------------------------
Object* SurfaceMesh::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = TriMesh::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_spkOrigVertices )
    {
        pkFound = m_spkOrigVertices->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( m_spkOrigIndices )
    {
        pkFound = m_spkOrigIndices->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( m_spkOrigParams )
    {
        pkFound = m_spkOrigParams->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    if ( m_aspkPatch )
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if ( m_aspkPatch[i] )
            {
                pkFound = m_aspkPatch[i]->GetObjectByID(uiID);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void SurfaceMesh::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iLevel);
    rkStream.Read(m_iFullVQuantity);
    rkStream.Read(m_iPatchQuantity);
    rkStream.Read(m_bAllowAttributes);
    rkStream.Read(m_bAllowDynamicChange);

    // link data
    Object* pkObject;

    rkStream.Read(pkObject);  // m_spkOrigVertices
    pkLink->Add(pkObject);
    bool bLocked = (pkObject == 0);

    rkStream.Read(pkObject);  // m_spkOrigIndices
    pkLink->Add(pkObject);

    rkStream.Read(pkObject);  // m_spkOrigParams
    pkLink->Add(pkObject);

    if ( !bLocked )
    {
        m_aspkPatch = new SurfacePatchPtr[m_iPatchQuantity];
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkPatch[i]
            pkLink->Add(pkObject);
        }
    }

    if ( m_bAllowAttributes && !bLocked )
    {
        m_akVTree = new VertexTree[m_iFullVQuantity];
        rkStream.Read(2*m_iFullVQuantity,(int*)m_akVTree);
    }

    if ( m_bAllowDynamicChange )
    {
        m_akSInfo = new SurfaceInfo[m_iFullVQuantity];
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Read(pkObject);
            pkLink->Add(pkObject);
            rkStream.Read(m_akSInfo[i].Param);
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(SurfaceMesh);
}
//----------------------------------------------------------------------------
void SurfaceMesh::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);

    Object* pkLinkID = pkLink->GetLinkID();
    m_spkOrigVertices = (Vector3fArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    m_spkOrigIndices = (IntArray*)rkStream.GetFromMap(pkLinkID);

    pkLinkID = pkLink->GetLinkID();
    m_spkOrigParams = (Vector2fArray*)rkStream.GetFromMap(pkLinkID);

    if ( m_aspkPatch )
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_aspkPatch[i] = (SurfacePatch*)rkStream.GetFromMap(pkLinkID);
        }
    }

    if ( m_akSInfo )
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_akSInfo[i].Patch = (SurfacePatch*)rkStream.GetFromMap(pkLinkID);
        }
    }
}
//----------------------------------------------------------------------------
bool SurfaceMesh::Register (Stream& rkStream) const
{
    if ( !TriMesh::Register(rkStream) )
        return false;

    if ( m_spkOrigVertices )
        m_spkOrigVertices->Register(rkStream);

    if ( m_spkOrigIndices )
        m_spkOrigIndices->Register(rkStream);

    if ( m_spkOrigParams )
        m_spkOrigParams->Register(rkStream);

    if ( m_aspkPatch )
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
        {
            if ( m_aspkPatch[i] )
                m_aspkPatch[i]->Register(rkStream);
        }
    }

    if ( m_akSInfo )
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            if ( m_akSInfo[i].Patch )
                m_akSInfo[i].Patch->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void SurfaceMesh::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_iLevel);
    rkStream.Write(m_iFullVQuantity);
    rkStream.Write(m_iPatchQuantity);
    rkStream.Write(m_bAllowAttributes);
    rkStream.Write(m_bAllowDynamicChange);

    // link data
    rkStream.Write(m_spkOrigVertices);
    rkStream.Write(m_spkOrigIndices);
    rkStream.Write(m_spkOrigParams);

    if ( m_aspkPatch )
    {
        for (int i = 0; i < m_iPatchQuantity; i++)
            rkStream.Write(m_aspkPatch[i]);
    }

    if ( m_akVTree )
        rkStream.Write(2*m_iFullVQuantity,(const int*)m_akVTree);

    if ( m_akSInfo )
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Write(m_akSInfo[i].Patch);
            rkStream.Write(m_akSInfo[i].Param);
        }
    }

    WM3_END_DEBUG_STREAM_SAVE(SurfaceMesh);
}
//----------------------------------------------------------------------------
StringTree* SurfaceMesh::SaveStrings (const char*)
{
    int iCQuantity = 1;
    if ( m_spkOrigVertices )
        iCQuantity++;
    if ( m_spkOrigIndices )
        iCQuantity++;
    if ( m_spkOrigParams )
        iCQuantity++;
    int i, iTQuantity = m_spkOrigIndices->GetQuantity()/3;
    for (i = 0; i < iTQuantity; i++)
    {
        if ( m_aspkPatch[i] )
            iCQuantity++;
    }

    StringTree* pkTree = new StringTree(3,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("level =",m_iLevel));
    pkTree->SetString(1,StringTree::Format("locked =",IsLocked()));

    // children
    pkTree->SetChild(0,TriMesh::SaveStrings());
    iCQuantity = 1;
    if ( m_spkOrigVertices )
    {
        pkTree->SetChild(iCQuantity++,
            m_spkOrigVertices->SaveStrings("original vertices"));
    }
    if ( m_spkOrigIndices )
    {
        pkTree->SetChild(iCQuantity++,
            m_spkOrigIndices->SaveStrings("original indices"));
    }
    if ( m_spkOrigParams )
    {
        pkTree->SetChild(iCQuantity++,
            m_spkOrigParams->SaveStrings("original parameters"));
    }
    for (i = 0; i < iTQuantity; i++)
    {
        if ( m_aspkPatch[i] )
            pkTree->SetChild(iCQuantity++,m_aspkPatch[i]->SaveStrings());
    }

    return pkTree;
}
//----------------------------------------------------------------------------
int SurfaceMesh::GetMemoryUsed () const
{
    int iSize = sizeof(SurfaceMesh) - sizeof(TriMesh) +
        TriMesh::GetMemoryUsed();

    if ( m_aspkPatch )
        iSize += m_iPatchQuantity*sizeof(m_aspkPatch[0]);

    if ( m_akVTree )
        iSize += m_iFullVQuantity*sizeof(m_akVTree[0]);

    if ( m_akSInfo )
        iSize += m_iFullVQuantity*sizeof(m_akSInfo[0]);

    return iSize;
}
//----------------------------------------------------------------------------
int SurfaceMesh::GetDiskUsed () const
{
    int iSize = TriMesh::GetDiskUsed() +
        sizeof(m_iLevel) +
        sizeof(m_iFullVQuantity) +
        sizeof(m_iPatchQuantity) +
        sizeof(char) +  // m_bAllowAttributes
        sizeof(char) +  // m_bAllowDynamicChange
        sizeof(m_spkOrigVertices) +
        sizeof(m_spkOrigIndices) +
        sizeof(m_spkOrigParams);

    if ( m_aspkPatch )
        iSize += m_iPatchQuantity*sizeof(m_aspkPatch[0]);

    if ( m_akVTree )
        iSize += m_iFullVQuantity*sizeof(m_akVTree[0]);

    if ( m_akSInfo )
        iSize += m_iFullVQuantity*sizeof(m_akSInfo[0]);

    return iSize;
}
//----------------------------------------------------------------------------
