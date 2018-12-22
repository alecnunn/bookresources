// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3CurveMesh.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,CurveMesh,Polyline);
WM3_IMPLEMENT_STREAM(CurveMesh);

//----------------------------------------------------------------------------
CurveMesh::CurveMesh (int iSegmentQuantity, CurveSegmentPtr* aspkSegment,
    FloatArrayPtr spkParams, bool bAllowAttributes, bool bAllowDynamicChange)
    :
    Polyline(new Vector3fArray(2,new Vector3f[2]),false,true)
{
    assert( iSegmentQuantity > 0 && aspkSegment );
    assert( spkParams && spkParams->GetData() );
    assert( spkParams->GetQuantity() == 2*iSegmentQuantity );

    // zero-out the default polyline vertices
    Vector3f* akVertex = Vertices->GetData();
    memset(akVertex,0,2*sizeof(Vector3f));

    m_iSegmentQuantity = iSegmentQuantity;
    m_aspkSegment = aspkSegment;
    m_spkOrigParams = spkParams;
    m_iLevel = 0;
    m_iFullVQuantity = 0;  // determined by SetLevel
    m_bAllowAttributes = bAllowAttributes;
    m_akVTree = 0;
    m_bAllowDynamicChange = bAllowDynamicChange;
    m_akCInfo = 0;

    SetLevel(-1);
}
//----------------------------------------------------------------------------
CurveMesh::CurveMesh ()
{
    m_iSegmentQuantity = 0;
    m_aspkSegment = 0;
    m_iLevel = 0;
    m_iFullVQuantity = 0;
    m_bAllowAttributes = false;
    m_akVTree = 0;
    m_bAllowDynamicChange = false;
    m_akCInfo = 0;
}
//----------------------------------------------------------------------------
CurveMesh::~CurveMesh ()
{
    delete[] m_aspkSegment;
    delete[] m_akVTree;
    delete[] m_akCInfo;
}
//----------------------------------------------------------------------------
void CurveMesh::SetLevel (int iLevel)
{
    if ( !m_spkOrigParams )
    {
        // the mesh is locked, no subdivision allowed
        return;
    }

    if ( iLevel < 0 )
        m_iLevel = 0;
    else if ( iLevel != m_iLevel )
        m_iLevel = iLevel;
    else  // input level is current level, nothing to do
        return;

    // allocate the storage for subdivision
    int iVQTotal, iEQTotal;
    Vector3f* akVertex;
    Edge* akEdge;
    Allocate(iVQTotal,akVertex,iEQTotal,akEdge);

    // subdivide the mesh
    int iVQ = m_iSegmentQuantity + 1;
    int iEQ = m_iSegmentQuantity;
    int i;
    for (i = 0; i < m_iLevel; i++)
    {
        int iVQNext = iVQ + iEQ;
        int iEQNext = 2*iEQ;
        Subdivide(iVQ,akVertex,iEQ,akEdge);
        assert( iVQ == iVQNext && iEQ == iEQNext );
        iVQ = iVQNext;
        iEQ = iEQNext;
    }

    // check to make sure the implementation is correct
    assert( iVQ == iVQTotal && iEQ == iEQTotal );
    m_iActiveQuantity = m_iFullVQuantity = iVQTotal;

    // create the polyline for the subdivision
    Vertices = new Vector3fArray(iVQTotal,akVertex);
    UpdateModelBound();

    int* aiIndex = new int[iVQTotal];
    int* piIndex = aiIndex;
    for (i = 0; i < iEQTotal; i++)
    {
        *piIndex++ = akEdge[i].V[0];
    }
    *piIndex++ = akEdge[iEQTotal-1].V[1];
    Indices = new IntArray(iVQTotal,aiIndex);

    delete[] akEdge;

    OnDynamicChange();
}
//----------------------------------------------------------------------------
void CurveMesh::Allocate (int& riVQTotal, Vector3f*& rakVertex,
    int& riEQTotal, Edge*& rakEdge)
{
    // the number of original vertices
    int iVQOrig = m_iSegmentQuantity + 1;

    // the number of original edges
    int iEQOrig = m_iSegmentQuantity;

    // determine the various quantities needed for subdivision
    riVQTotal = iVQOrig;
    riEQTotal = iEQOrig;
    int i;
    for (i = 0; i < m_iLevel; i++)
    {
        riVQTotal = riVQTotal + riEQTotal;
        riEQTotal = 2*riEQTotal;
    }

    // allocate storage for the subdivision
    rakVertex = new Vector3f[riVQTotal];
    rakEdge = new Edge[riEQTotal];
    if ( m_bAllowAttributes )
    {
        delete[] m_akVTree;
        m_akVTree = new VertexTree[riVQTotal];
    }
    if ( m_bAllowDynamicChange )
    {
        delete[] m_akCInfo;
        m_akCInfo = new CurveInfo[riVQTotal];
        InitializeCurveInfo();
    }

    // compute the end point vertices
    const float* afParam = m_spkOrigParams->GetData();
    for (i = 0; i < m_iSegmentQuantity; i++)
    {
        rakVertex[i] = m_aspkSegment[i]->P(afParam[2*i]);
    }
    rakVertex[m_iSegmentQuantity] = m_aspkSegment[m_iSegmentQuantity-1]->P(
        afParam[2*m_iSegmentQuantity-1]);

    // initialize the edge data structures
    int iIndex = 0;
    for (i = 0; i < iEQOrig; i++)
    {
        Edge& rkE = rakEdge[i];

        // curve affecting the edge
        rkE.Segment = m_aspkSegment[i];

        // get the vertex indices for the edge
        rkE.V[0] = iIndex++;
        rkE.V[1] = iIndex;

        // get the parameters associated with the vertices
        rkE.Param[0] = *afParam++;
        rkE.Param[1] = *afParam++;
    }
}
//----------------------------------------------------------------------------
void CurveMesh::Subdivide (int& riVQ, Vector3f* akVertex, int& riEQ,
    Edge* akEdge)
{
    // The subdivision is computed in-place, so you need to fill in the new
    // edge slots starting from the end of the array and working towards the
    // beginning.  This avoid overwriting old edges not yet subdivided.
    for (int iOld = riEQ-1, iCurr = 2*riEQ-1; iOld >= 0; iOld--)
    {
        Edge& rkE = akEdge[iOld];

        // compute the vertex at the average of the parameter values
        float fParamMid = 0.5f*(rkE.Param[0] + rkE.Param[1]);
        akVertex[riVQ] = rkE.Segment->P(fParamMid);

        if ( m_bAllowAttributes )
        {
            m_akVTree[riVQ].V[0] = rkE.V[0];
            m_akVTree[riVQ].V[1] = rkE.V[1];
        }

        if ( m_bAllowDynamicChange )
        {
            m_akCInfo[riVQ].Segment = rkE.Segment;
            m_akCInfo[riVQ].Param = fParamMid;
        }

        // subdivide the edge
        Edge& rkE1 = akEdge[iCurr--];
        rkE1.Segment = rkE.Segment;
        rkE1.V[0] = riVQ;
        rkE1.V[1] = rkE.V[1];
        rkE1.Param[0] = fParamMid;
        rkE1.Param[1] = rkE.Param[1];

        Edge& rkE0 = akEdge[iCurr--];
        rkE0.Segment = rkE.Segment;
        rkE0.V[0] = rkE.V[0];
        rkE0.V[1] = riVQ;
        rkE0.Param[0] = rkE.Param[0];
        rkE0.Param[1] = fParamMid;

        riVQ++;
    }

    riEQ = 2*riEQ;
}
//----------------------------------------------------------------------------
float* CurveMesh::GetAttributes (int iSize, const float* afAttr) const
{
    if ( !m_bAllowAttributes )
        return 0;

    int iOrigVQuantity = m_iSegmentQuantity + 1;
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
void CurveMesh::OnDynamicChange ()
{
    if ( m_bAllowDynamicChange )
    {
        Vector3f* akVertex = Vertices->GetData();
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            CurveInfo& rkCI = m_akCInfo[i];
            akVertex[i] = rkCI.Segment->P(rkCI.Param);
        }

        UpdateMS();
    }
}
//----------------------------------------------------------------------------
void CurveMesh::Lock ()
{
    if ( m_spkOrigParams )
    {
        m_spkOrigParams = 0;
        delete[] m_aspkSegment;
        m_aspkSegment = 0;
        delete[] m_akVTree;
        m_akVTree = 0;
    }

    // The array m_akCInfo is not deleted.  This allows dynamic updates of
    // the vertices of the mesh, even though the mesh is "locked".
}
//----------------------------------------------------------------------------
void CurveMesh::InitializeCurveInfo ()
{
    // Because vertices are shared by edges, the last visited edge for a
    // vertex donates its segment and parameter values to that vertex.
    const float* afParam = m_spkOrigParams->GetData();
    int iIndex = 0;
    for (int i = 0; i < m_iSegmentQuantity; i++)
    {
        int iV0 = iIndex++;
        int iV1 = iIndex;
        m_akCInfo[iV0].Segment = m_aspkSegment[i];
        m_akCInfo[iV0].Param = *afParam++;
        m_akCInfo[iV1].Segment = m_aspkSegment[i];
        m_akCInfo[iV1].Param = *afParam++;
    }
}
//----------------------------------------------------------------------------
CurveMesh::Edge::Edge ()
{
    Segment = 0;
    V[0] = -1;
    V[1] = -1;
    Param[0] = 0.0f;
    Param[1] = 0.0f;
}
//----------------------------------------------------------------------------
CurveMesh::VertexTree::VertexTree ()
{
    V[0] = -1;
    V[1] = -1;
}
//----------------------------------------------------------------------------
CurveMesh::CurveInfo::CurveInfo ()
{
    Segment = 0;
    Param = 0.0f;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// name and unique id
//----------------------------------------------------------------------------
Object* CurveMesh::GetObjectByName (const String& rkName)
{
    Object* pkFound = Polyline::GetObjectByName(rkName);
    if ( pkFound )
        return pkFound;

    if ( m_aspkSegment )
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if ( m_aspkSegment[i] )
            {
                pkFound = m_aspkSegment[i]->GetObjectByName(rkName);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    if ( m_spkOrigParams )
    {
        pkFound = m_spkOrigParams->GetObjectByName(rkName);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------
void CurveMesh::GetAllObjectsByName (const String& rkName,
    TArray<Object*>& rkObjects)
{
    Polyline::GetAllObjectsByName(rkName,rkObjects);

    if ( m_aspkSegment )
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if ( m_aspkSegment[i] )
                m_aspkSegment[i]->GetAllObjectsByName(rkName,rkObjects);
        }
    }

    if ( m_spkOrigParams )
        m_spkOrigParams->GetAllObjectsByName(rkName,rkObjects);
}
//----------------------------------------------------------------------------
Object* CurveMesh::GetObjectByID (unsigned int uiID)
{
    Object* pkFound = Polyline::GetObjectByID(uiID);
    if ( pkFound )
        return pkFound;

    if ( m_aspkSegment )
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if ( m_aspkSegment[i] )
            {
                pkFound = m_aspkSegment[i]->GetObjectByID(uiID);
                if ( pkFound )
                    return pkFound;
            }
        }
    }

    if ( m_spkOrigParams )
    {
        pkFound = m_spkOrigParams->GetObjectByID(uiID);
        if ( pkFound )
            return pkFound;
    }

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void CurveMesh::Load (Stream& rkStream, Stream::Link* pkLink)
{
    WM3_BEGIN_DEBUG_STREAM_LOAD;

    Polyline::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_iSegmentQuantity);
    rkStream.Read(m_iLevel);
    rkStream.Read(m_iFullVQuantity);
    rkStream.Read(m_bAllowAttributes);
    rkStream.Read(m_bAllowDynamicChange);

    // link data
    Object* pkObject;
    rkStream.Read(pkObject);  // m_spkOrigParams
    pkLink->Add(pkObject);
    bool bLocked = (pkObject == 0);

    if ( !bLocked )
    {
        m_aspkSegment = new CurveSegmentPtr[m_iSegmentQuantity];
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            rkStream.Read(pkObject);  // m_aspkSegment[i]
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
        m_akCInfo = new CurveInfo[m_iFullVQuantity];
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Read(pkObject);
            pkLink->Add(pkObject);
            rkStream.Read(m_akCInfo[i].Param);
        }
    }

    WM3_END_DEBUG_STREAM_LOAD(CurveMesh);
}
//----------------------------------------------------------------------------
void CurveMesh::Link (Stream& rkStream, Stream::Link* pkLink)
{
    Polyline::Link(rkStream,pkLink);

    Object* pkLinkID;
    if ( m_aspkSegment )
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_aspkSegment[i] = (CurveSegment*)rkStream.GetFromMap(pkLinkID);
        }
    }

    pkLinkID = pkLink->GetLinkID();
    m_spkOrigParams = (FloatArray*)rkStream.GetFromMap(pkLinkID);

    if ( m_akCInfo )
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            pkLinkID = pkLink->GetLinkID();
            m_akCInfo[i].Segment =
                (CurveSegment*)rkStream.GetFromMap(pkLinkID);
        }
    }
}
//----------------------------------------------------------------------------
bool CurveMesh::Register (Stream& rkStream) const
{
    if ( !Polyline::Register(rkStream) )
        return false;

    if ( m_aspkSegment )
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
        {
            if ( m_aspkSegment[i] )
                m_aspkSegment[i]->Register(rkStream);
        }
    }

    if ( m_spkOrigParams )
        m_spkOrigParams->Register(rkStream);

    if ( m_akCInfo )
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            if ( m_akCInfo[i].Segment )
                m_akCInfo[i].Segment->Register(rkStream);
        }
    }

    return true;
}
//----------------------------------------------------------------------------
void CurveMesh::Save (Stream& rkStream) const
{
    WM3_BEGIN_DEBUG_STREAM_SAVE;

    Polyline::Save(rkStream);

    // native data
    rkStream.Write(m_iSegmentQuantity);
    rkStream.Write(m_iLevel);
    rkStream.Write(m_iFullVQuantity);
    rkStream.Write(m_bAllowAttributes);
    rkStream.Write(m_bAllowDynamicChange);

    // link data
    if ( m_aspkSegment )
    {
        for (int i = 0; i < m_iSegmentQuantity; i++)
            rkStream.Write(m_aspkSegment[i]);
    }

    rkStream.Write(m_spkOrigParams);

    if ( m_akVTree )
        rkStream.Write(2*m_iFullVQuantity,(int*)m_akVTree);

    if ( m_akCInfo )
    {
        for (int i = 0; i < m_iFullVQuantity; i++)
        {
            rkStream.Write(m_akCInfo[i].Segment);
            rkStream.Write(m_akCInfo[i].Param);
        }
    }

    WM3_END_DEBUG_STREAM_SAVE(CurveMesh);
}
//----------------------------------------------------------------------------
StringTree* CurveMesh::SaveStrings (const char*)
{
    int i, iCQuantity = 1;

    if ( m_aspkSegment )
    {
        for (i = 0; i < m_iSegmentQuantity; i++)
        {
            if ( m_aspkSegment[i] )
                iCQuantity++;
        }
    }
    if ( m_spkOrigParams )
        iCQuantity++;

    StringTree* pkTree = new StringTree(3,iCQuantity);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("level =",m_iLevel));
    pkTree->SetString(1,StringTree::Format("locked =",IsLocked()));

    // children
    pkTree->SetChild(0,Polyline::SaveStrings());
    iCQuantity = 1;
    if ( m_aspkSegment )
    {
        for (i = 0; i < m_iSegmentQuantity; i++)
        {
            if ( m_aspkSegment[i] )
            {
                pkTree->SetChild(iCQuantity++,
                    m_aspkSegment[i]->SaveStrings());
            }
        }
    }
    if ( m_spkOrigParams )
    {
        pkTree->SetChild(iCQuantity++,
            m_spkOrigParams->SaveStrings("original parameters"));
    }

    return pkTree;
}
//----------------------------------------------------------------------------
int CurveMesh::GetMemoryUsed () const
{
    int iSize = sizeof(CurveMesh) - sizeof(Polyline) +
        Polyline::GetMemoryUsed();

    if ( m_aspkSegment )
        iSize += m_iSegmentQuantity*sizeof(m_aspkSegment[0]);

    if ( m_akVTree )
        iSize += m_iFullVQuantity*sizeof(m_akVTree[0]);

    if ( m_akCInfo )
        iSize += m_iFullVQuantity*sizeof(m_akCInfo[0]);

    return iSize;
}
//----------------------------------------------------------------------------
int CurveMesh::GetDiskUsed () const
{
    int iSize = Polyline::GetDiskUsed() +
        sizeof(m_iSegmentQuantity) +
        sizeof(m_iLevel) +
        sizeof(m_iFullVQuantity) +
        sizeof(char) +  // m_bAllowAttributes
        sizeof(char) +  // m_bAllowDynamicChange
        sizeof(m_spkOrigParams);

    if ( m_aspkSegment )
        iSize += m_iSegmentQuantity*sizeof(m_aspkSegment[0]);

    if ( m_akVTree )
        iSize += m_iFullVQuantity*sizeof(m_akVTree[0]);

    if ( m_akCInfo )
        iSize += m_iFullVQuantity*sizeof(m_akCInfo[0]);

    return iSize;
}
//----------------------------------------------------------------------------
