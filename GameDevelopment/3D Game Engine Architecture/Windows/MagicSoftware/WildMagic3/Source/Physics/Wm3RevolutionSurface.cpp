// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#include "Wm3RevolutionSurface.h"
#include "Wm3StandardMesh.h"
using namespace Wm3;

WM3_IMPLEMENT_RTTI(Wm3,RevolutionSurface,TriMesh);
WM3_IMPLEMENT_STREAM(RevolutionSurface);
WM3_IMPLEMENT_DEFAULT_NAME_ID(RevolutionSurface,TriMesh);

//----------------------------------------------------------------------------
RevolutionSurface::RevolutionSurface (Curve2f* pkCurve, float fXCenter,
    int eTopology, int iCurveSamples, int iRadialSamples, bool bWantNormals,
    bool bWantColors, bool bWantUVs, bool bSampleByArcLength,
    bool bOutsideView)
{
    assert( pkCurve );
    m_pkCurve = pkCurve;
    m_fXCenter = fXCenter;
    m_eTopology = eTopology;
    m_iCurveSamples = iCurveSamples;
    m_iRadialSamples = iRadialSamples;
    m_bSampleByArcLength = bSampleByArcLength;

    ComputeSampleData();

    // The topology of the meshes is all that matters.  The vertices will be
    // modified later based on the curve of revolution.
    StandardMesh kSM;
    kSM.Normals(bWantNormals,false).UVs(bWantUVs,false).Inside(!bOutsideView);

    TriMesh* pkMesh = 0;
    switch ( m_eTopology )
    {
    case REV_DISK_TOPOLOGY:
        pkMesh = kSM.Disk(m_iCurveSamples,m_iRadialSamples,1.0f);
        break;
    case REV_CYLINDER_TOPOLOGY:
        pkMesh = kSM.Cylinder(m_iCurveSamples,m_iRadialSamples,1.0f,1.0f,true);
        break;
    case REV_SPHERE_TOPOLOGY:
        pkMesh = kSM.Sphere(m_iCurveSamples,m_iRadialSamples,1.0f);
        break;
    case REV_TORUS_TOPOLOGY:
        pkMesh = kSM.Torus(m_iCurveSamples,m_iRadialSamples,1.0f,0.25f);
        break;
    }
    assert( pkMesh );

    Vertices = pkMesh->Vertices;
    Normals = pkMesh->Normals;
    Indices = pkMesh->Indices;
    delete pkMesh;

    // create the effect
    Vector2fArrayPtr spkUVs = kSM.GetUVs();
    if ( bWantColors || spkUVs )
    {
        Effect* pkEffect = new Effect;

        if ( bWantColors )
        {
            int iVQuantity = Vertices->GetQuantity();
            ColorRGB* akColor = new ColorRGB[iVQuantity];
            memset(akColor,0,iVQuantity*sizeof(ColorRGB));
            pkEffect->ColorRGBs = new ColorRGBArray(iVQuantity,akColor);
        }

        if ( spkUVs )
            pkEffect->UVs.Append(spkUVs);

        SetEffect(pkEffect);
    }

    // generate the actual surface by replacing the vertex values
    UpdateSurface();
}
//----------------------------------------------------------------------------
RevolutionSurface::RevolutionSurface ()
{
    m_pkCurve = 0;
    m_afSin = 0;
    m_afCos = 0;
    m_akSample = 0;
}
//----------------------------------------------------------------------------
RevolutionSurface::~RevolutionSurface ()
{
    delete[] m_afSin;
    delete[] m_afCos;
    delete[] m_akSample;
}
//----------------------------------------------------------------------------
void RevolutionSurface::ComputeSampleData ()
{
    // Compute slice vertex coefficients.  The first and last coefficients
    // are duplicated to allow a closed cross section that has two different
    // pairs of texture coordinates at the shared vertex.
    m_afSin = new float[m_iRadialSamples+1];
    m_afCos = new float[m_iRadialSamples+1];
    float fInvRS = 1.0f/(float)m_iRadialSamples;
    for (int i = 0; i < m_iRadialSamples; i++)
    {
        float fAngle = Mathf::TWO_PI*fInvRS*i;
        m_afCos[i] = Mathf::Cos(fAngle);
        m_afSin[i] = Mathf::Sin(fAngle);
    }
    m_afSin[m_iRadialSamples] = m_afSin[0];
    m_afCos[m_iRadialSamples] = m_afCos[0];

    // allocate storage for curve samples
    m_akSample = new Vector3f[m_iCurveSamples];
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateSurface ()
{
    // parameters for evaluating curve
    float fTMin = m_pkCurve->GetMinTime();
    float fTRange = m_pkCurve->GetMaxTime() - fTMin;

    // sampling by arc length requires the total length of the curve
    float fTotalLength;
    if ( m_bSampleByArcLength )
        fTotalLength = m_pkCurve->GetTotalLength();
    else
        fTotalLength = 0.0f;

    // sample the curve of revolution
    float fInvCSm1 = 1.0f/(float)(m_iCurveSamples - 1);
    for (int i = 0; i < m_iCurveSamples; i++)
    {
        float fT;
        if ( m_bSampleByArcLength )
            fT = m_pkCurve->GetTime(i*fTotalLength*fInvCSm1);
        else
            fT = fTMin + i*fTRange*fInvCSm1;

        Vector2f kPos = m_pkCurve->GetPosition(fT);
        m_akSample[i].X() = kPos.X();
        m_akSample[i].Y() = 0.0f;
        m_akSample[i].Z() = kPos.Y();
    }

    // Store the samples and their rotated equivalents.  The storage layout
    // is dependent on the topology of the mesh.
    switch ( m_eTopology )
    {
    case REV_DISK_TOPOLOGY:
        UpdateDisk();
        break;
    case REV_CYLINDER_TOPOLOGY:
        UpdateCylinder();
        break;
    case REV_SPHERE_TOPOLOGY:
        UpdateSphere();
        break;
    case REV_TORUS_TOPOLOGY:
        UpdateTorus();
        break;
    }

    UpdateModelNormals();
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateDisk ()
{
    // initial ray
    Vector3f* akVertex = Vertices->GetData();
    int iC;
    for (iC = 0; iC < m_iCurveSamples; iC++)
        akVertex[iC] = m_akSample[iC];

    // remaining rays obtained by revolution
    int iCSm1 = m_iCurveSamples - 1;
    for (int iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 1; iC < m_iCurveSamples; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if ( fRadius < 0.0f )
                fRadius = 0.0f;
            int i = iC+iCSm1*iR;
            akVertex[i].X() = m_fXCenter + fRadius*m_afCos[iR];
            akVertex[i].Y() = fRadius*m_afSin[iR];
            akVertex[i].Z() = m_akSample[iC].Z();
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateSphere ()
{
    int iVQuantity = Vertices->GetQuantity();
    Vector3f* akVertex = Vertices->GetData();

    // south pole
    akVertex[iVQuantity-2] = m_akSample[0];

    // north pole
    akVertex[iVQuantity-1] = m_akSample[m_iCurveSamples-1];

    // initial and final ray
    int iC, i;
    for (iC = 1; iC <= m_iCurveSamples-2; iC++)
    {
        i = (iC-1)*(m_iRadialSamples+1);
        akVertex[i] = m_akSample[iC];

        i += m_iRadialSamples;
        akVertex[i] = m_akSample[iC];
    }

    // remaining rays obtained by revolution
    for (int iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 1; iC <= m_iCurveSamples-2; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if ( fRadius < 0.0f )
                fRadius = 0.0f;
            i = (iC-1)*(m_iRadialSamples+1) + iR;
            akVertex[i].X() = m_fXCenter + fRadius*m_afCos[iR];
            akVertex[i].Y() = fRadius*m_afSin[iR];
            akVertex[i].Z() = m_akSample[iC].Z();
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateCylinder ()
{
    // initial and final ray
    Vector3f* akVertex = Vertices->GetData();
    int iC, i;
    for (iC = 0; iC < m_iCurveSamples; iC++)
    {
        i = iC*(m_iRadialSamples+1);
        akVertex[i] = m_akSample[iC];

        i += m_iRadialSamples;
        akVertex[i] = m_akSample[iC];
    }

    // remaining rays obtained by revolution
    for (int iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 0; iC < m_iCurveSamples; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if ( fRadius < 0.0f )
                fRadius = 0.0f;
            i = iC*(m_iRadialSamples+1) + iR;
            akVertex[i].X() = m_fXCenter + fRadius*m_afCos[iR];
            akVertex[i].Y() = fRadius*m_afSin[iR];
            akVertex[i].Z() = m_akSample[iC].Z();
        }
    }
}
//----------------------------------------------------------------------------
void RevolutionSurface::UpdateTorus ()
{
    // initial and final ray
    int iVQuantity = Vertices->GetQuantity();
    Vector3f* akVertex = Vertices->GetData();
    int iC, i;
    for (iC = 0; iC < m_iCurveSamples; iC++)
    {
        i = iC*(m_iRadialSamples+1);
        akVertex[i] = m_akSample[iC];

        i += m_iRadialSamples;
        akVertex[i] = m_akSample[iC];
    }

    // remaining rays obtained by revolution
    int iR;
    for (iR = 1; iR < m_iRadialSamples; iR++)
    {
        for (iC = 0; iC < m_iCurveSamples; iC++)
        {
            float fRadius = m_akSample[iC].X() - m_fXCenter;
            if ( fRadius < 0.0f )
                fRadius = 0.0f;
            i = iC*(m_iRadialSamples+1) + iR;
            akVertex[i].X() = m_fXCenter + fRadius*m_afCos[iR];
            akVertex[i].Y() = fRadius*m_afSin[iR];
            akVertex[i].Z() = m_akSample[iC].Z();
        }
    }

    i = iVQuantity - (m_iRadialSamples+1);
    for (iR = 0; iR <= m_iRadialSamples; iR++, i++)
        akVertex[i] = akVertex[iR];
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// streaming
//----------------------------------------------------------------------------
void RevolutionSurface::Load (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Load(rkStream,pkLink);

    // native data
    rkStream.Read(m_fXCenter);
    rkStream.Read(m_eTopology);
    rkStream.Read(m_iCurveSamples);
    rkStream.Read(m_iRadialSamples);
    rkStream.Read(m_iRadialSamples+1,m_afSin);
    rkStream.Read(m_iRadialSamples+1,m_afCos);
    rkStream.Read(m_bSampleByArcLength);

    // TO DO.  See note in RevolutionSurface::Save.
    m_pkCurve = 0;
}
//----------------------------------------------------------------------------
void RevolutionSurface::Link (Stream& rkStream, Stream::Link* pkLink)
{
    TriMesh::Link(rkStream,pkLink);
}
//----------------------------------------------------------------------------
bool RevolutionSurface::Register (Stream& rkStream) const
{
    return TriMesh::Register(rkStream);
}
//----------------------------------------------------------------------------
void RevolutionSurface::Save (Stream& rkStream) const
{
    TriMesh::Save(rkStream);

    // native data
    rkStream.Write(m_fXCenter);
    rkStream.Write(m_eTopology);
    rkStream.Write(m_iCurveSamples);
    rkStream.Write(m_iRadialSamples);
    rkStream.Write(m_iRadialSamples+1,m_afSin);
    rkStream.Write(m_iRadialSamples+1,m_afCos);
    rkStream.Write(m_bSampleByArcLength);

    // TO DO.  The class Curve2 is abstract and does not know about the data
    // representation for the derived-class object that is passed to the
    // RevolutionSurface constructor.  Because of this, any loaded
    // RevolutionSurface object will require the application to manually set
    // the curve via the Curve() member.
    //
    // Streaming support should probably be added to the curve classes and
    // a mechanism for saving function pointers should probably be created.
}
//----------------------------------------------------------------------------
StringTree* RevolutionSurface::SaveStrings (const char*)
{
    StringTree* pkTree = new StringTree(6,1);

    // strings
    pkTree->SetString(0,StringTree::Format(&TYPE,GetName()));
    pkTree->SetString(1,StringTree::Format("x center = ",m_fXCenter));
    switch ( m_eTopology )
    {
    case REV_CYLINDER_TOPOLOGY:
        pkTree->SetString(2,StringTree::Format("topology = CYLINDER"));
        break;
    case REV_DISK_TOPOLOGY:
        pkTree->SetString(2,StringTree::Format("topology = DISK"));
        break;
    case REV_SPHERE_TOPOLOGY:
        pkTree->SetString(2,StringTree::Format("topology = SPHERE"));
        break;
    case REV_TORUS_TOPOLOGY:
        pkTree->SetString(2,StringTree::Format("topology = TORUS"));
        break;
    }
    pkTree->SetString(3,StringTree::Format("curve samples = ",
        m_iCurveSamples));
    pkTree->SetString(4,StringTree::Format("radial samples = ",
        m_iRadialSamples));
    pkTree->SetString(5,StringTree::Format("sample by arc length = ",
        m_bSampleByArcLength));

    // children
    pkTree->SetChild(0,TriMesh::SaveStrings());

    return pkTree;
}
//----------------------------------------------------------------------------
int RevolutionSurface::GetMemoryUsed () const
{
    // TO DO.  Does not count the size of the Curve2 object.  The curve class
    // is virtual, so that system would need to provide a GetBytesUsed member
    // function.
    return sizeof(RevolutionSurface) - sizeof(TriMesh) +
        TriMesh::GetMemoryUsed() +
        2*(m_iRadialSamples+1)*sizeof(float);  // m_afSin, m_afCos
}
//----------------------------------------------------------------------------
int RevolutionSurface::GetDiskUsed () const
{
    return TriMesh::GetDiskUsed() +
        sizeof(char) + // m_eTopology)
        sizeof(m_iCurveSamples) +
        sizeof(m_iRadialSamples) +
        2*(m_iRadialSamples+1)*sizeof(float) +  // m_afSin, m_afCos
        sizeof(char);  // m_bSampleByArcLength
}
//----------------------------------------------------------------------------
